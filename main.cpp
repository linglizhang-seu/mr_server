#include <QCoreApplication>
#include "manageserver.h"
//传入的apo需要重新保存，使得n按顺序
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ManageServer server;
    if(!server.listen(QHostAddress::Any,9999))
    {
        qDebug()<<"Error:cannot start server in port 9999,please check!";
        exit(-1);
    }else
    {
        qDebug()<<"server(2.0) for vr_farm started!";
    }
    return a.exec();
}

/*
 *
 * 1. 发送所有用户:users:ji;ji;ji
 */
