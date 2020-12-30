#include "messagesocket.h"
#include <QDataStream>
#include <QFile>
#include <QRegExp>
#include <QHostAddress>
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
    qDebug()<<socket->peerAddress().toString()<<socket->peerPort();
    connect(socket,&QTcpSocket::readyRead,this,&MessageSocket::onreadyRead);
    connect(socket,&QTcpSocket::disconnected,this,&MessageSocket::disconnected);
}

void MessageSocket::onreadyRead()
{
    try {
        qDebug()<<"Message:"+socket->peerAddress().toString()+"onread";
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
                qDebug()<<"Message:"+socket->peerAddress().toString()<<" datasize = "<<dataInfo.stringOrFilenameSize<<" "<<dataInfo.filedataSize;
                if(dataInfo.stringOrFilenameSize>=1024*1000||dataInfo.filedataSize>=1024*1024*100)
                {
                    socket->disconnectFromHost();
                    while(!socket->waitForDisconnected());
                    this->deleteLater();
                }
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
            processReaded(list);
        }else
            return;
        onreadyRead();
    }  catch (std::bad_alloc&) {
        qDebug()<<"Message "<<socket->peerAddress().toString()<<" can not get available raw for read!";
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
void MessageSocket::resetDataInfo()
{
     dataInfo.dataSize=0;dataInfo.stringOrFilenameSize=0;
     dataInfo.dataReadedSize=0;dataInfo.filedataSize=0;
}

void MessageSocket::sendMsg(const QString & msg)
{
    qint32 stringSize=msg.toUtf8().size();
    qint32 totalsize=3*sizeof (qint32)+stringSize;
    QByteArray block;
    QDataStream dts(&block,QIODevice::WriteOnly);
    dts<<qint32(totalsize)<<qint32(stringSize)<<qint32(0);
    block+=msg.toUtf8();
    socket->write(block);
    socket->flush();
}

void MessageSocket::sendFiles(QStringList filePathList,QStringList fileNameList)
{
    int totalsize=sizeof(qint32);
    QList<QByteArray> blocks;
    for(int i=0;i<filePathList.size();i++)
    {
        QByteArray block;
        block.clear();
        QDataStream dts(&block,QIODevice::WriteOnly);
        QFile f(filePathList[i]);
        if(!f.open(QIODevice::ReadOnly))
            qDebug()<<"cannot open file:"<<filePathList[i]<<" "<<f.errorString();
        QByteArray fileName=fileNameList[i].toUtf8();
        QByteArray fileData=f.readAll();
        f.close();
        dts<<qint32(fileName.size())<<qint32(fileData.size());
        block=block+fileName;
        block=block+fileData;
        blocks.push_back(block);
        totalsize+=block.size();
    }
    QByteArray block;

    block.clear();
    QDataStream dts(&block,QIODevice::WriteOnly);
    dts<<qint32(totalsize);
    for(int i=0;i<blocks.size();i++)
        block=block+blocks[i];
    qDebug()<<totalsize<<' '<<block.size();
    socket->write(block);
    socket->flush();

//    for(auto filepath:filePathList)
//    {
//        if(filepath.contains("/tmp/"))
//        {
//            QFile(filepath).remove();
//        }
//    }

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
    QRegExp msgRex("^/(.*)_(.*):(.*)");
//    QRegExp drawlineRex("^/drawline:(.*)$");
//    QRegExp dellineRex("^/delline:(.*)");
//    QRegExp addmarkerRex("^/addmarker(.*)");
//    QRegExp delmarkerRex("^/delmarker(.*)");

//    QRegExp retypelineRex("^/retypeline:(.*)");

//    QRegExp retypemarkerRex("^/retypemarker:(.*)");//unused
    if(loginRex.indexIn(msg)!=-1)
    {
        emit userLogin(msg);
    }else if(msgRex.indexIn(msg)!=-1)
    {
        emit pushMsg(msg);
    }
//    else if(drawlineRex.indexIn(msg)!=-1)
//    {
//        emit pushMsg(msg);
//    }else if(dellineRex.indexIn(msg)!=-1)
//    {
//        emit pushMsg(msg);
//    }else if(addmarkerRex.indexIn(msg)!=-1)
//    {
//        emit pushMsg(msg);
//    }else if(delmarkerRex.indexIn(msg)!=-1)
//    {
//        emit pushMsg(msg);
//    }else if(retypelineRex.indexIn(msg)!=-1)
//    {
//        emit pushMsg(msg);
//    }else if(retypemarkerRex.indexIn(msg)!=-1)
//    {
//        emit pushMsg(msg);
//    }

}
