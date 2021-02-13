#include "messagesocket.h"
#include <QDataStream>
#include <QFile>
#include <QRegExp>
#include <QHostAddress>
#include <QCoreApplication>
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
bool MessageSocket::processMsg(const QString rmsg)
{
    if(!rmsg.endsWith('\n')) return true;
    QRegExp loginRex("^/login:(.*)$");
    QRegExp ImgBlockRex("^/Imgblock:(.*)");
    QRegExp GetBBSWCRex("^/GetBBSwc:(.*)");
    QRegExp msgRex("^/(.*)_(.*):(.*)");

    QString msg=rmsg.trimmed();
    qDebug()<<this<<"message:"<<msg;
    if(loginRex.indexIn(msg)!=-1)
    {
        emit userLogin(loginRex.cap(1));
    }else if(ImgBlockRex.indexIn(msg)!=-1)
    {
        sendFiles({QCoreApplication::applicationDirPath()+"/tmp/test_128_128_128.v3draw"},{"test_128_128_128.v3draw"});
    }else if(GetBBSWCRex.indexIn(msg)!=-1)
    {
        emit getBBSWC(GetBBSWCRex.cap(1));
    }else if(msgRex.indexIn(msg)!=-1)
    {
        emit pushMsg(msg);
    }
    else {
        return false;
    }
    return true;
}

bool MessageSocket::processFile(const QString filepath)
{
    return true;
}
