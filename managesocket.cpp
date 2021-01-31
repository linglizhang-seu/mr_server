#include "managesocket.h"
#include <QDataStream>
#include <QSqlQuery>
#include <QFile>
#include <QCoreApplication>
#include <QRegExp>
#include <QTime>
#include "basicdatamanage.h"
#include "messageserver.h"

bool ManageSocket::processMsg( const QString rmsg)
{
    if(!rmsg.endsWith('\n')) return false;
    qDebug()<<"receive msg:"<<rmsg;
    QString msg=rmsg.trimmed();
    if(msg.startsWith("LOGIN:"))
    {
        QString data=msg.right(msg.size()-QString("LOGIN:").size());
        QStringList loginInfo=data.split(' ');
        //登陆验证
    }else if(msg.startsWith("REGISTER:"))
    {
        QString data=msg.right(msg.size()-QString("REGISTER:").size());
        QStringList registerInfo=data.split(' ');
        //注册验证
    }else if(msg.startsWith("FORGETPASSWORD:"))
    {
        QString data=msg.right(msg.size()-QString("FORGETPASSWORD:").size());
        //密码找回验证
    }else if(msg.startsWith("GETFILELIST:"))
    {
        QString conPath=msg.right(msg.size()-QString("GETFILELIST:").size());
        //获取conpath文件夹下文件列表
    }else if(msg.startsWith("LOADFILES:"))
    {
        QString conPath=msg.right(msg.size()-QString("GETFILELIST:").size());
        //加载
    }else if(msg.startsWith("GETALLACTIVECollABORATE"))
    {
        //获取当前所有的协作列表
    }else{
        return false;
    }
    return true;

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
