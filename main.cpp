#include <QCoreApplication>
#include <stdlib.h>
#include <QMutex>
#include <QDateTime>
#include <QFile>
#include <cstdlib>
#include <cstdio>
#include "manageserver.h"
//传入的apo需要重新保存，使得n按顺序
#include "basicdatamanage.h"

//传入的apo需要重新保存，使得n按顺序
QString vaa3dPath;
QMap<QString,QStringList> m_MapImageIdWithRes;
QMap<QString,QString> m_MapImageIdWithDir;
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // 加锁
    static QMutex mutex;
    mutex.lock();

    QByteArray localMsg = msg.toLocal8Bit();

    // 设置输出信息格式
    QString strDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss : ");
//    QString strMessage = QString("%1 File:%2  Line:%3  Function:%4  DateTime:%5\n")
//            .arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function).arg(strDateTime);
    QString strMessage=strDateTime+localMsg.constData()+"\n";
    // 输出信息至文件中（读写、追加形式）
    QFile file("log.txt");
    file.open(QIODevice::ReadWrite | QIODevice::Append);
    QTextStream stream(&file);
    stream << strMessage ;
    file.flush();

    file.close();
    // 解锁
    mutex.unlock();
    fprintf(stderr,strMessage.toStdString().c_str());
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageOutput);
    QCoreApplication a(argc, argv);
    vaa3dPath=QCoreApplication::applicationDirPath()+"/vaa3d";
    QStringList list18454={
        "RES(26298x35000x11041)",
        "RES(13149x17500x5520)",
        "RES(6574x8750x2760)",
        "RES(3287x4375x1380)",
        "RES(1643x2187x690)",
        "RES(821x1093x345)"};
    m_MapImageIdWithRes.insert("18454",list18454);
    m_MapImageIdWithDir.insert("18454","18454");
    ManageServer server;
    if(!server.listen(QHostAddress::Any,23763))
    {
        qDebug()<<"Error:cannot start server in port 9999,please check!";
        exit(-1);
    }else
    {
        if(!DB::createTableForUser())
        {
            qDebug()<<"mysql error";
            exit(-1);
        }else
        {
            qDebug()<<"server(2.0.5.0) for vr_farm started!";
        }
    }
    return a.exec();
}

