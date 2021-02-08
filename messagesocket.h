#ifndef MESSAGESOCKET_H
#define MESSAGESOCKET_H

#include "tcpsocket.h"
class MessageSocket : public TcpSocket
{
    Q_OBJECT
public:
    explicit MessageSocket(qintptr handle,QObject *parent = nullptr)
        :TcpSocket(handle,parent){}
public slots:
    void sendfiles(MessageSocket* socket,QStringList filepath);
    void sendmsgs(MessageSocket* socket,QStringList msglist);
    void disconnectName(MessageSocket * p)
    {
        if(this==p)
        {
            this->socket->disconnectFromHost();
            while(this->socket->state()!=QTcpSocket::UnconnectedState)
                this->socket->waitForDisconnected();
        }
    }
    void slotSendMsg(const QString & str)
    {
        sendMsg(str);
    }
signals:
    void pushMsg(QString );
    void userLogin(QString);
    void getBBSWC(QString);
private:
    bool processMsg(const QString msg);
    bool processFile(const QString filepath);
//    void sendFiles(QStringList filePathList,QStringList fileNameList);

    //message processor
};

#endif // MESSAGESOCKET_H
