#ifndef SIMCLIENT_H
#define SIMCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QProcess>
#include <basic_c_fun/basic_surf_objs.h>
#include <neuron_editing/neuron_format_converter.h>

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
        socket=new QTcpSocket;
        socket->connectToHost(ip,port.toInt());
        if(socket->waitForConnected())
        {
            sendMsg(QString("/login:" +id));

            for(int i=0;i<msgs.size();i++)
                sendMsg(msgs[i]);
        }
    }


private:

    void sendMsg(QString msg)
    {
        qint32 stringSize=msg.toUtf8().size();
        qint32 totalsize=3*sizeof (qint32)+stringSize;
        QByteArray block;
        QDataStream dts(&block,QIODevice::WriteOnly);
        dts<<qint32(totalsize)<<qint32(stringSize)<<qint32(0);
        block+=msg.toUtf8();
        socket->write(block);
        socket->flush();
        qDebug()<<"send to server:"<<msg;
    }

    QTcpSocket *socket;
    QString ip;
    QString port;
    QString id;
    QStringList msgs;
};

#endif // SIMCLIENT_H
