#include "messagesocket.h"
#include <QDataStream>
#include <QFile>
#include <QRegExp>
#include <QHostAddress>
#include <QCoreApplication>
#include "basicdatamanage.h"
#include <QMap>
extern QMap<QString,QStringList> m_MapImageIdWithRes;
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
    QRegExp ImageResRex("^/ImageRes:(.*)");
    QRegExp msgRex("^/(.*)_(.*):(.*)");

    QString msg=rmsg.trimmed();
//    qDebug()<<this<<"message:"<<msg;
    if(loginRex.indexIn(msg)!=-1)
    {
        emit userLogin(loginRex.cap(1));
    }else if(ImgBlockRex.indexIn(msg)!=-1)
    {
        auto name_path=IP::getImageBlock(ImgBlockRex.cap(1).trimmed()+QString::number(this->socket->socketDescriptor()));
        sendFiles({name_path.at(1)},{name_path.at(0)});
    }else if(GetBBSWCRex.indexIn(msg)!=-1)
    {
        if(msg=="/GetBBSwc:COLL")
            emit getBBSWC("");
        else emit getBBSWC(GetBBSWCRex.cap(1).trimmed()+QString::number(this->socket->socketDescriptor()));
    }else if(ImageResRex.indexIn(msg)!=-1)
    {
        QString id=ImageResRex.cap(1).trimmed();
        sendMsg("ImgRes:"+id+";"+QString::number(IP::getImageRes(id))+";"+m_MapImageIdWithRes.value(id).join(';'));
    }
    else if(msgRex.indexIn(msg)!=-1)
    {
        emit pushMsg(msg);
    }else if (msg=="GETSCORE")
    {
        emit getscore();
    }else if(msg.startsWith("SETSOCRE:"))
    {
        emit setscore(msg.right(msg.size()-QString("SETSOCRE:").size()).toUInt());
    }
    else if(msg!="HeartBeat:")
    {
        return false;
    }
    return true;
}

bool MessageSocket::processFile(const QString filepath)
{
    return true;
}
