#include "messagesocket.h"
#include <QDataStream>
#include <QFile>
#include <QRegExp>
#include <QHostAddress>
#include <QCoreApplication>
#include <QDir>
MessageSocket::MessageSocket(qintptr handle,QObject *parent) : QObject(parent)
{
    socket=nullptr;
    socketDescriptor=handle;
    resetDataType();
}

void MessageSocket::onstarted()
{
    socket=new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);
    qDebug()<<socket->peerAddress().toString()<<socket->peerPort();
    connect(socket,&QTcpSocket::readyRead,this,&MessageSocket::onreadyRead);
    connect(socket,&QTcpSocket::disconnected,this,&MessageSocket::disconnected);
}

void MessageSocket::onreadyRead()
{
    if(!datatype.isFile)
    {
        if(socket->canReadLine())
        {
            QString msg=socket->readLine();
            if(!msg.endsWith('\n'))
            {
                socket->disconnectFromHost();
                this->deleteLater();
            }else
            {
                msg=msg.trimmed();
                QRegExp reg("FILENAMESIZE:(.*)");
                if(reg.indexIn(msg)!=-1)
                {
                    datatype.isFile=true;
                    auto fileNameAndSize=reg.cap(1).split("*;*");
                    if(fileNameAndSize.size()!=2)
                    {
                        socket->disconnectFromHost();
                        this->deleteLater();
                    }
                    datatype.filename=fileNameAndSize[0];
                    datatype.filesize=fileNameAndSize[1].toLongLong();
                }else{
                    QStringList list;
                    list.push_back("00"+msg);
                    processReaded(list);
                }
                onreadyRead();
            }
        }
    }
    else{
        if(socket->bytesAvailable()>=datatype.filesize)
        {
            if(!QDir(QCoreApplication::applicationDirPath()+"/tmp").exists())
                QDir(QCoreApplication::applicationDirPath()).mkdir("tmp");
            QString filePath=QCoreApplication::applicationDirPath()+"/tmp/"+datatype.filename;
            QFile file(filePath);
            file.open(QIODevice::WriteOnly);
            int length=file.write(socket->read(datatype.filesize));
            if(length!=datatype.filesize)
            {
                qDebug()<<"Error:read file";
            }
            file.flush();
            file.close();
            QStringList list;
            list.push_back("11"+filePath);
            resetDataType();
            processReaded(list);
            onreadyRead();
        }
    }
}

void MessageSocket::processReaded(QStringList list)
{
    for(auto msg:list)
    {
        if(msg.startsWith("00"))
        {
            msg=msg.remove(0,2);
            qDebug()<<this->socket->peerAddress().toString()<<":"<<msg;
            processMsg(msg);
        }
    }
}
void MessageSocket::resetDataType()
{
    datatype.isFile=false;
    datatype.filesize=0;
    datatype.filename.clear();
}

void MessageSocket::sendMsg(const QString & msg)
{    
    QString data=msg+"\n";
    int length=socket->write(data.toStdString().c_str(),data.size());
    if(data.size()!=length)
        qDebug()<<"Error:send "+data;
    socket->flush();
}

void MessageSocket::sendFiles(QStringList filePathList,QStringList fileNameList)
{
    for(int i=0;i<filePathList.size();i++)
    {
        QFile f(filePathList[i]);
        if(!f.open(QIODevice::ReadOnly))
        {
            qDebug()<<"Manage:cannot open file "<<fileNameList[i]<<" "<<f.errorString();
            return ;
        }
        QByteArray fileData=f.readAll();
        sendMsg("FILENAMESIZE:"+(fileNameList[i]+"*;*"+QString::number(fileData.size())));
        int length=socket->write(fileData);
        if(length!=fileData.size())
        {
            qDebug()<<"Error:send data";
        }
        socket->flush();
    }
    for(auto filepath:filePathList)
    {
        if(filepath.contains("/tmp/"))
        {
            QFile(filepath).remove();
        }
    }

}


void MessageSocket::sendfiles(MessageSocket* socket,QStringList filepaths)
{
    if(socket==this)
    {
        QStringList filenames;
        for(auto filepath:filepaths)
        {
            filenames.push_back(filepath.section('/',-1));
        }
        sendFiles(filepaths,filenames);
    }
}

void MessageSocket::sendmsgs(MessageSocket* socket,QStringList msgs)
{
    if(socket==this)
    {
        for(auto msg:msgs)
        {
            sendMsg(msg);
        }
    }
}
void MessageSocket::processMsg(const QString msg)
{
    QRegExp loginRex("^/login:(.*)$");
    QRegExp msgRex("^/(.*)_(.*):(.*)$");

    if(loginRex.indexIn(msg)!=-1)
    {
        emit userLogin(loginRex.cap(1));
    }else if(msgRex.indexIn(msg)!=-1)
    {
        emit pushMsg(msg);
    }
}
