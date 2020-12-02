#include "messagesocket.h"
#include <QDataStream>
#include <QFile>
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
    if(dataInfo.dataReadedSize==0&&socket->bytesAvailable()>=sizeof (qint32))
    {
        QDataStream in(socket);
        in>>dataInfo.dataSize;dataInfo.dataReadedSize+=sizeof (qint32);
        if(dataInfo.dataSize<0) {
            qDebug()<<"error";return;
        }
        if(dataInfo.dataSize==dataInfo.dataReadedSize)
        {
            resetDataInfo();return;
        }

        if(socket->bytesAvailable()>=dataInfo.dataSize-dataInfo.dataReadedSize)
        {
            QStringList list;
            while(dataInfo.dataSize!=dataInfo.dataReadedSize)
            {
                in>>dataInfo.stringOrFilenameSize>>dataInfo.filedataSize;
                QString messageOrFileName=QString::fromUtf8(socket->read(dataInfo.stringOrFilenameSize),dataInfo.stringOrFilenameSize);
                if(dataInfo.filedataSize==0)
                {
                    list.push_back(messageOrFileName);
                }
                dataInfo.dataReadedSize+=(2*sizeof (qint32)+dataInfo.stringOrFilenameSize+dataInfo.filedataSize);
            }
            resetDataInfo();
            processMsg(list[0]);
        }
    }else
    {
        if(socket->bytesAvailable()>=dataInfo.dataSize-dataInfo.dataReadedSize)
        {
            QDataStream in(socket);
            QStringList list;
            while(dataInfo.dataSize!=dataInfo.dataReadedSize)
            {
                in>>dataInfo.stringOrFilenameSize>>dataInfo.filedataSize;
                QString messageOrFileName=QString::fromUtf8(socket->read(dataInfo.stringOrFilenameSize),dataInfo.stringOrFilenameSize);
                if(dataInfo.filedataSize==0)
                {
                    list.push_back(messageOrFileName);
                }
                dataInfo.dataReadedSize+=(2*sizeof (qint32)+dataInfo.stringOrFilenameSize+dataInfo.filedataSize);
            }
            resetDataInfo();
            processMsg(list[0]);
        }
    }
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
    QDataStream dts(&block1,QIODevice::WriteOnly);
    for(int i=0;i<filePathList.size();i++)
    {
        auto fileName=fileNameList[i].toUtf8();
        auto fileData=QFile(filePathList[i]).readAll();
        totalsize += 2* sizeof (qint32);
        totalsize += fileName.size();
        totalsize += fileData.size();
        dts<<qint32(fileName.size())<<qint32(fileData.size());
        block1 += fileName +=fileData;
    }
    QByteArray block;
    QDataStream dts1(&block,QIODevice::WriteOnly);
    dts<<qint32(totalsize);
    block+=block1;
    socket->write(block);
    socket->waitForBytesWritten();

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
void MessageSocket::processMsg(const QString msg)
{

}
