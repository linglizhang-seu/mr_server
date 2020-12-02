#include <QCoreApplication>
#include "manageserver.h"
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
