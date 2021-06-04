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
    qDebug()<<manageSokcet<<" "<<manageSokcet->username <<" connect";

}


