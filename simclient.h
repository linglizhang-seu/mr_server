#ifndef SIMCLIENT_H
#define SIMCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QProcess>
#include <basic_c_fun/basic_surf_objs.h>
#include <neuron_editing/neuron_format_converter.h>
#include <iostream>
class SimClient:public QObject
{
    Q_OBJECT
public:
    SimClient(QString ip,QString port,QString id,QStringList msgs):ip(ip),port(port),id(id),msgs(msgs)
    {
        socket=nullptr;
    }
public slots:
    void onstarted()
    {
        if(!on) return;
        on=false;
        socket=new QTcpSocket;
        connect(socket,SIGNAL(readyRead()),this,SLOT(onRead()));

        socket->connectToHost(ip,port.toInt());
        while(!socket->waitForConnected());

        sendMsg(QString("/login:" +id));
        for(int i=0;i<msgs.size();i++)
            sendMsg(msgs[i]);
        socket->flush();
        sendMsg("");
        QObject::disconnect(QThread::currentThread(),
                            SIGNAL(started()),this,SLOT(onstarted()));
//        socket->disconnectFromHost();
//        while(!(socket->state() == QAbstractSocket::UnconnectedState
//               || socket->waitForDisconnected(1000))) ;
        qDebug()<<id <<" has complete!";
        QThread::currentThread()->yieldCurrentThread();
    }

    void onRead(){
        socket->readAll();
    }


private:

    void sendMsg(QString msg)
    {
        qint32 stringSize=msg.toUtf8().size();
        qint32 rawsize=msg.toStdString().size();
        qint32 tsize=QByteArray::fromStdString(msg.toStdString()).size();
        qint32 totalsize=3*sizeof (qint32)+stringSize;
        QByteArray block;
        QDataStream dts(&block,QIODevice::WriteOnly);
        dts<<qint32(totalsize);
        dts<<qint32(stringSize);
        dts<<qint32(0);
        block+=msg.toUtf8();
        qint64 sendedsize=socket->write(block);
        if(sendedsize!=totalsize)
            qDebug()<<QString("Error:send!=total,%1,%2").arg(sendedsize).arg(totalsize);
        socket->flush();
        socket->waitForBytesWritten(60*1000);
//        qDebug()<<"sendToServer:"<<block;
    }

    QTcpSocket *socket;
    QString ip;
    QString port;
    QString id;
    QStringList msgs;
    bool on=true;
};

#endif // SIMCLIENT_H
