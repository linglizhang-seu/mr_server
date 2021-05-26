#include "manageserver.h"
#include "managesocket.h"
#include "ThreadPool.h"
#include "ThreadPool.h"
#include <QHostAddress>
ManageServer::ManageServer(QObject *parent):QTcpServer(parent)
{

}

void ManageServer::incomingConnection(qintptr handle)
{
    ManageSocket * manageSokcet = new ManageSocket(handle);
    qDebug()<<manageSokcet <<"connect";
    QObject::connect(manageSokcet,&TcpSocket::tcpdisconnected,this,[=]{
        if(!manageSokcet->username.isEmpty())
              ManageSocket::onLineUsers.remove(manageSokcet->username);
         manageSokcet->deleteLater();
    },Qt::DirectConnection);
}

