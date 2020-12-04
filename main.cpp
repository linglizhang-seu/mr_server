#include <QCoreApplication>
#include "manageserver.h"
//传入的apo需要重新保存，使得n按顺序
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ManageServer server;
    if(!server.listen(QHostAddress::Any,9999))
    {
        exit(-1);
    }
    return a.exec();
}
