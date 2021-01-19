#ifndef MESSAGESOCKET_H
#define MESSAGESOCKET_H

#include <QTcpSocket>
class MessageSocket : public QObject
{
    Q_OBJECT
    struct DataType{
        bool isFile=false;//false msg,true file
        qint64 filesize=0;
        QString filename;
    };
public:
    explicit MessageSocket(qintptr handle,QObject *parent = nullptr);
    QTcpSocket *socket;
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

    qintptr socketDescriptor;
    DataType datatype;
private:
    void resetDataType();
    void processMsg(const QString msg);
    void sendFiles(QStringList filePathList,QStringList fileNameList);
    void processReaded(QStringList list);
};



#endif // MESSAGESOCKET_H
