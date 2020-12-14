#include "messagesocket.h"
#include <QDataStream>
#include <QFile>
#include <QRegExp>
MessageSocket::MessageSocket(qintptr handle,QObject *parent) : QObject(parent)
{
    socket=nullptr;
    socketDescriptor=handle;
    resetDataInfo();
}

void MessageSocket::onstarted()
{
    socket=new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket,&QTcpSocket::readyRead,this,&MessageSocket::onreadyRead);
    connect(socket,&QTcpSocket::disconnected,this,&MessageSocket::disconnected);
}

void MessageSocket::onreadyRead()
{
    QDataStream in(socket);
    if(dataInfo.dataSize==0)
    {
        if(socket->bytesAvailable()>=sizeof (qint32))
        {
            in>>dataInfo.dataSize;
            dataInfo.dataReadedSize+=sizeof (qint32);
        }
        else return;
    }

    if(dataInfo.stringOrFilenameSize==0&&dataInfo.filedataSize==0)
    {
        if(socket->bytesAvailable()>=2*sizeof (qint32))
        {
            in>>dataInfo.stringOrFilenameSize>>dataInfo.filedataSize;
            dataInfo.dataReadedSize+=(2*sizeof (qint32));
        }else
            return;
    }
    QStringList list;
    if(socket->bytesAvailable()>=dataInfo.stringOrFilenameSize+dataInfo.filedataSize)
    {
        QString messageOrFileName=QString::fromUtf8(socket->read(dataInfo.stringOrFilenameSize),dataInfo.stringOrFilenameSize);

        if(dataInfo.filedataSize)
        {
            qDebug()<<"error :filedatasize !=0";
            socket->disconnectFromHost();
        }else
        {
            list.push_back("00"+messageOrFileName);
        }
        dataInfo.dataReadedSize+=(dataInfo.stringOrFilenameSize+dataInfo.filedataSize);
        dataInfo.stringOrFilenameSize=0;
        dataInfo.filedataSize=0;
        if(dataInfo.dataReadedSize==dataInfo.dataSize)
            resetDataInfo();
        processMsg(list[0]);
    }else
        return;
    onreadyRead();
}
void MessageSocket::resetDataInfo()
{
     dataInfo.dataSize=0;dataInfo.stringOrFilenameSize=0;
     dataInfo.dataReadedSize=0;dataInfo.filedataSize=0;
}

void MessageSocket::sendMsg(QString msg)
{
    qint32 stringSize=msg.toUtf8().size();
    qint32 totalsize=3*sizeof (qint32)+stringSize;
    QByteArray block;
    QDataStream dts(&block,QIODevice::WriteOnly);
    dts<<qint32(totalsize)<<qint32(stringSize)<<qint32(0);
    block+=msg.toUtf8();
    socket->write(block);
    socket->waitForBytesWritten();
}

void MessageSocket::sendFiles(QStringList filePathList,QStringList fileNameList)
{
    qint32 totalsize=sizeof (qint32);
    QByteArray block1;
    {
        QDataStream dts(&block1,QIODevice::WriteOnly);
        for(int i=0;i<filePathList.size();i++)
        {
            auto fileName=fileNameList[i].toUtf8();
            QFile f(filePathList[i]);
            f.open(QIODevice::ReadOnly);
            auto fileData=f.readAll();
            f.flush();
            f.close();
            totalsize += 2* sizeof (qint32);
            totalsize += fileName.size();
            totalsize += fileData.size();
            dts<<qint32(fileName.size())<<qint32(fileData.size());
            block1=block1+fileName;
            block1=block1+fileData;
        }
    }
    QByteArray block;
    QDataStream dts1(&block,QIODevice::WriteOnly);
    dts1<<qint32(totalsize);
    qDebug()<<totalsize;
    block=block+block1;
    socket->write(block);
    socket->waitForBytesWritten();
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

    QRegExp drawlineRex("^/drawline:(.*)$");
    QRegExp dellineRex("^/delline:(.*)");
    QRegExp addmarkerRex("^/addmarker(.*)");
    QRegExp delmarkerRex("^/delmakrer(.*)");

    QRegExp retypelineRex("^/retypeline:(.*)");

    QRegExp retypemarkerRex("^/retypemarker:(.*)");//unused

    if(drawlineRex.indexIn(msg)!=-1)
    {
        emit pushMsg(msg,0);
    }else if(dellineRex.indexIn(msg)!=-1)
    {
        emit pushMsg(msg,0);
    }else if(addmarkerRex.indexIn(msg)!=-1)
    {
        emit pushMsg(msg,1);
    }else if(delmarkerRex.indexIn(msg)!=-1)
    {
        emit pushMsg(msg,0);
    }else if(retypelineRex.indexIn(msg)!=-1)
    {
        emit pushMsg(msg,0);
    }else if(retypemarkerRex.indexIn(msg)!=-1)
    {
        emit pushMsg(msg,0);
    }else if(loginRex.indexIn(msg)!=-1)
    {
        emit userlogin(dellineRex.cap(1).trimmed());
    }

}
