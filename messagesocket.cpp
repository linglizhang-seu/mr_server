#include "messagesocket.h"
#include <QDataStream>
#include <QFile>
#include <QRegExp>
#include <QHostAddress>

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
bool MessageSocket::processMsg(const QString msg)
{
    if(!msg.endsWith('\n')) return true;
    QRegExp loginRex("^/login:(.*)$");
    QRegExp msgRex("^/(.*)_(.*):(.*)");

    if(loginRex.indexIn(msg)!=-1)
    {
        emit userLogin(loginRex.cap(1));
    }else if(msgRex.indexIn(msg)!=-1)
    {
        emit pushMsg(msg);
    }
}

bool MessageSocket::processFile(const QString filepath)
{
    return true;
}
