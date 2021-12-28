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

        sendMsg({QString("/Login:" +id)});

        for(int i=0;i<msgs.size();i++)
            sendMsg({msgs[i]});
        socket->flush();
        sendMsg({""});
        QObject::disconnect(QThread::currentThread(),
                            SIGNAL(started()),this,SLOT(onstarted()));
        qDebug()<<id <<" has complete!";
    }

    void onRead(){
        socket->readAll();
    }


private:

    void sendMsg(QStringList msgs)
    {

        if(socket->state()!=QAbstractSocket::ConnectedState)
            return;

        const std::string data=msgs.join(';').toStdString();
        const std::string header=QString("DataTypeWithSize:%1 %2\n").arg(0).arg(data.size()).toStdString();
        socket->write(header.c_str(),header.size());
        socket->write(data.c_str(),data.size());
        socket->flush();

    }

    QTcpSocket *socket;
    QString ip;
    QString port;
    QString id;
    QStringList msgs;
    bool on=true;
};

#endif // SIMCLIENT_H
