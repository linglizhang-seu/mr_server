#ifndef MESSAGESOCKET_H
#define MESSAGESOCKET_H

#include "tcpsocket.h"
class MessageSocket : public TcpSocket
{
    Q_OBJECT
public:
    explicit MessageSocket(qintptr handle,QObject *parent = nullptr)
        :TcpSocket(handle,parent){}
    ~MessageSocket() {
        qDebug()<<(MessageSocket*)this <<" " <<this->username<<" delete";
    }
public slots:
    void sendfiles(MessageSocket* socket,QStringList filepath);
    void sendmsgs(MessageSocket* socket,QStringList msglist);
    void disconnectName(MessageSocket * p);
    void slotSendMsg(const QString & str);

signals:
    void pushMsg(QString );
    void userLogin(QString);
    void getBBSWC(QString);
    void getscore();
    void setscore(int);
    void ackNeuron(QString);
private:
    bool processMsg(const QString msg);
    bool processFile(const QString filepath);
//    void sendFiles(QStringList filePathList,QStringList fileNameList);

    //message processor
};

#endif // MESSAGESOCKET_H
