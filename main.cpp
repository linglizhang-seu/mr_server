#include <QCoreApplication>
#include <QString>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if(argc<5) return -1;;
    int port=atoi(argv[1]);
    QString iamge=argv[2];
    QString neuron=argv[3];
    QString anoname=argv[4];






    return a.exec();
}


