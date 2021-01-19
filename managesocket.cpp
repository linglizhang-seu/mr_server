#include "managesocket.h"
#include <QDataStream>
#include <QSqlQuery>
#include <QFile>
#include <QCoreApplication>
#include <QRegExp>
#include <QTime>
#include "basicdatamanage.h"
#include "messageserver.h"

ManageSocket::ManageSocket(qintptr handle,QObject *parent):QObject(parent)
{
    {
        socket=nullptr;
        socketDescriptor=handle;
        resetDataType();
        msgs.clear();
        filepaths.clear();
    }
    socket=new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);
    qDebug()<<this<<" "<<socket->peerAddress().toString()<<" "<<socket->peerPort();
    connect(socket,&QTcpSocket::readyRead,this,&ManageSocket::onreadyRead);
    connect(socket,&QTcpSocket::disconnected,[=]
    {
        this->deleteLater();
        qDebug()<<"Manage:"+socket->peerAddress().toString()+"delete";
    });

}

void ManageSocket::onreadyRead()
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

void ManageSocket::resetDataType()
{
    datatype.isFile=false;
    datatype.filesize=0;
    datatype.filename.clear();
}

void ManageSocket::sendMsg(const QString type,const QString msg)
{
    QString data=type+":"+msg+"\n";
    int length=socket->write(data.toStdString().c_str(),data.size());
    if(data.size()!=length)
        qDebug()<<"Error:send "+data;
    socket->flush();
}

void ManageSocket::sendFiles(const QStringList filePathList,const QStringList fileNameList)
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
        sendMsg("FILENAMESIZE",(fileNameList[i]+"*;*"+QString::number(fileData.size())));
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

void ManageSocket::processReaded(QStringList list)
{
    for(auto msg:list)
    {
        if(msg.startsWith("00"))
        {
            processFile(filepaths);
            msgs.push_back(msg.remove(0,2));
        }else if(msg.startsWith("11"))
        {
            processMsg(msgs);
             filepaths.push_back(msg.remove(0,2));
        }
    }
    processMsg(msgs);
    processFile(filepaths);
}
void ManageSocket::processMsg( QStringList &msgs)
{
     for(auto msg:msgs)
     {
         qDebug()<<"receive msg:"<<msg;
        QRegExp Download("Download:(.*)");//;;;;:Download
        QRegExp LoadANO("LoadANO:(.*)");//17302_00001:LoadANO
        QRegExp FileList("CurrentFiles:(.*)");//data:CurrentFiles

        if(Download.indexIn(msg)!=-1)
        {
            auto pathMapName=FE::getFilesPathFormFileName(Download.cap(1).trimmed());
            sendFiles(pathMapName.firstKey(),pathMapName.value(pathMapName.firstKey()));
        }else if(LoadANO.indexIn(msg)!=-1)
        {
            auto p=makeMessageServer(LoadANO.cap(1).trimmed());
            auto port=p?p->port:"-1";
            sendMsg("Port",port);
        }else if(FileList.indexIn(msg)!=-1)
        {
            //返回当前所有文件的列表
            QString dirname=FileList.cap(1).trimmed().split(";").at(1).trimmed();
            QStringList datafileNames=FE::getFileNames(dirname);
            sendMsg("CurrentFiles",FileList.cap(1)+";"+datafileNames.join(";"));
        }
    }
     msgs.clear();
}
void ManageSocket::processFile( QStringList &filePaths)
{
    FE::processFileFromClient(filePaths);
    filePaths.clear();
}

MessageServer* ManageSocket::makeMessageServer(QString neuron)
{
    return MessageServer::makeMessageServer(neuron);
}
