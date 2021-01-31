#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QFile>

class TcpSocket : public QObject
{
    Q_OBJECT
    struct DataType{
        bool isFile=false;//false msg,true file
        qint64 datasize=0;
        QString filename;
        QFile *f=nullptr;
    };
public:
    explicit TcpSocket(qintptr handle,QObject *parent = nullptr);
    virtual ~TcpSocket()=default;
    virtual bool processMsg(const QString)=0;
    virtual bool processFile(const QString)=0;
    bool sendMsg(QString str);
    bool sendFiles(QStringList filePathList,QStringList fileNameList);
public slots:
    void onreadyRead();
public:
    QTcpSocket *socket=nullptr;
private:
    qintptr socketDescriptor;
    DataType datatype;

private:
    void resetDataType();
    bool processHeader(const QString msg);
    void errorprocess(int,QString msg="");

signals:
    void disconnected();

};

#endif // TCPSOCKET_H
