#ifndef MESSAGESOCKET_H
#define MESSAGESOCKET_H

#include <QTcpSocket>
class MessageSocket : public QObject
{
    Q_OBJECT
    struct DataInfo
    {
        qint32 dataSize;
        qint32 stringOrFilenameSize;
        qint32 filedataSize;
        qint32 dataReadedSize;
    };
public:
    explicit MessageSocket(qintptr handle,QObject *parent = nullptr);
public slots:
    void onstarted();
    void onreadyRead();
    void sendMsg(const QString & msg);
    void sendfiles(MessageSocket* socket,QStringList filepath);
    void sendmsgs(MessageSocket* socket,QStringList msglist);
signals:
    void disconnected();
    void pushMsg(QString );
    void userLogin(QString);
private:
    QTcpSocket *socket;
    qintptr socketDescriptor;
    DataInfo dataInfo;
private:
    void resetDataInfo();
    void processMsg(const QString msg);
    void sendFiles(QStringList filePathList,QStringList fileNameList);
    void processReaded(QStringList list);

    //message processor
};

/*
 * messagesocket
 * ----------------------
 * slot
 * + onstarted()
 * + onreadyread()
 * + sendMsg(QString)
 * + sendfiles(MessageSocket*,QStringList )
 * + sendmsgs(MessageSocket* ,QStringList )
 * signal
 * disconnected()
 * pushMsg(QString,bool)
 * userlogin(QString)
 * -------------------
 * - socket:QTcpsocket
 * - socketdescriptor:qintptr
 * - datainfo:DataInfo
 * -----------------------------
 * - resetdatainfo()
 * - processMsg(QString)
 * - sendFiles(QStringList,QStringList)
 */

#endif // MESSAGESOCKET_H
