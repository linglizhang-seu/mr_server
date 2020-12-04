#ifndef MANAGESOCKET_H
#define MANAGESOCKET_H

#include <QTcpSocket>
#include "messageserver.h"


class ManageSocket:public QObject
{
    struct DataInfo
    {
        qint32 dataSize;
        qint32 stringOrFilenameSize;
        qint32 filedataSize;
        qint32 dataReadedSize;
    };

    Q_OBJECT
public:
    ManageSocket(qintptr handle,QObject * parent=nullptr);

public slots:
    void onreadyRead();

private:
    QTcpSocket *socket;
    qintptr socketDescriptor;
    DataInfo dataInfo;
    QStringList msgs;
    QStringList filepaths;

private:
    void resetDataInfo();
    void sendMsg(QString msg);
    void sendFiles(QStringList filePathList,QStringList fileNameList);
    void processReaded(QStringList list);
    void processMsg( QStringList &msglist);
    void processFile( QStringList &filePaths);

//    QMap<QStringList,QStringList> getANOFILE(QString neuronid,bool& f);

    QString getMessageServerport(QString neuronid);
    //process file
    void processARBOR(QStringList filepaths,QStringList filenames);
    void processFULL(QStringList filepaths,QStringList filenames);
    void processOTHER(QStringList filepaths,QStringList filenames);

    MessageServer* makeMessageServer(QString neuron);
signals:
    void disconnected();
};

#endif // MANAGESOCKET_H
