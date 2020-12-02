#include "manageserver.h"
#include "managesocket.h"
#include "ThreadPool.h"
#include "ThreadPool.h"
ManageServer::ManageServer(QObject *parent):QTcpServer(parent)
{

}

void ManageServer::incomingConnection(qintptr handle)
{
    ManageSocket * manageSokcet = new ManageSocket(handle);
}

