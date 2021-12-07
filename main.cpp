#include <QCoreApplication>
#include <stdlib.h>
#include <QMutex>
#include <QDateTime>
#include <QFile>
#include <cstdlib>
#include <cstdio>
#include "manageserver.h"
//传入的apo需要重新保存，使得n按顺序

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
//    qInstallMessageHandler(myMessageOutput);
    QCoreApplication a(argc, argv);

    return a.exec();
}

