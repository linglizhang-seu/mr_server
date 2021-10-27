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

    void sendMsg(QString str)
    {
        if(socket->state()==QAbstractSocket::ConnectedState)
        {
            const QString data=str+"\n";
            int datalength=data.size();
            QString header=QString("DataTypeWithSize:%1;;%2\n").arg(0).arg(datalength);
            socket->write(header.toStdString().c_str(),header.size());
            socket->write(data.toStdString().c_str(),data.size());
            socket->flush();
        }
    }

    QTcpSocket *socket;
    QString ip;
    QString port;
    QString id;
    QStringList msgs;
};

#endif // SIMCLIENT_H
