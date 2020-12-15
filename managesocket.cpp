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
        resetDataInfo();
        msgs.clear();
        filepaths.clear();
    }
    socket=new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket,&QTcpSocket::readyRead,this,&ManageSocket::onreadyRead);
    connect(socket,&QTcpSocket::disconnected,[=]
    {
        this->deleteLater();
        qDebug()<<this<<"delete";
    });

}

void ManageSocket::onreadyRead()
{
    qDebug()<<"Manage on read";
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
        qDebug()<<messageOrFileName;
        if(dataInfo.filedataSize)
        {
            QString filePath=QCoreApplication::applicationDirPath()+"/tmp/"+messageOrFileName;
            QFile file(filePath);
            file.open(QIODevice::WriteOnly);
            file.write(socket->read(dataInfo.filedataSize));file.flush();
            file.close();
            list.push_back("11"+filePath);
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
}

void ManageSocket::resetDataInfo()
{
     dataInfo.dataSize=0;dataInfo.stringOrFilenameSize=0;
     dataInfo.dataReadedSize=0;dataInfo.filedataSize=0;
}

void ManageSocket::sendMsg(QString msg)
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

void ManageSocket::sendFiles(QStringList filePathList,QStringList fileNameList)
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
            qDebug()<<"cannot open file "<<fileNameList[i]<<" "<<f.errorString();
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
        QRegExp Download("(.*):Download");//;;;;:Download
        QRegExp LoadANO("(.*):LoadANO");//17302_00001:LoadANO
        QRegExp FileList("(.*):CurrentFiles");//data:CurrentFiles

        if(Download.indexIn(msg)!=-1)
        {
            auto pathMapName=FE::getFilesPathFormFileName(Download.cap(1).trimmed());
            sendFiles(pathMapName.firstKey(),pathMapName.value(pathMapName.firstKey()));
        }else if(LoadANO.indexIn(msg)!=-1)
        {

            auto p=makeMessageServer(LoadANO.cap(1).trimmed());
            auto port=p?p->port:"-1";
            sendMsg(port+":Port");
        }else if(FileList.indexIn(msg)!=-1)
        {
            //返回当前所有文件的列表
            QString dirname=FileList.cap(1).trimmed().split(";").at(1).trimmed();
            QStringList datafileNames=FE::getFileNames(dirname);
            sendMsg(datafileNames.join(";")+";"+msg);
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
