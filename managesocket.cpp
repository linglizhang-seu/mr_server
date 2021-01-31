#include "managesocket.h"
#include <QDataStream>
#include <QSqlQuery>
#include <QFile>
#include <QCoreApplication>
#include <QRegExp>
#include <QTime>
#include "basicdatamanage.h"
#include "messageserver.h"

bool ManageSocket::processMsg( const QString msg)
{
    if(!msg.endsWith('\n')) return false;
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
    }else{
        return false;
    }
    return true;
}

bool ManageSocket::processFile( const QString filePath)
{
    return FE::processFileFromClient({filePath});
}

MessageServer* ManageSocket::makeMessageServer(QString neuron)
{
    return MessageServer::makeMessageServer(neuron);
}
