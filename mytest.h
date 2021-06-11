#ifndef MYTEST_H
#define MYTEST_H

#include <QFile>
#include "simclient.h"
void test1(QString ip,QString port,int b,QString p)//100个机器人重建 发送消息，服务器处理的数据数量与时间的关系
{
    SimClient::cropImageAndRename(b,p);
    QFile f("center.txt");
    QStringList centers;
    if(f.open(QIODevice::ReadOnly))
    {
        while(!f.atEnd())
        {
            centers.push_back(f.readLine().trimmed());
        }
    }
    QVector<SimClient> clients;
    QThread *threads=new QThread[centers.size()];
    for(int i=0;i<centers.size();i++)
    {
        XYZ start;
        QStringList ss=centers[i].trimmed().split('_');
        start.x=ss.at(0).toFloat();
        start.y=ss.at(1).toFloat();
        start.z=ss.at(2).toFloat();
        clients.push_back(SimClient(ip,port,QString::number(i+1),start));
        clients[i].moveToThread(threads+i);
        QObject::connect(threads+i,&QThread::started,&(clients[i]),&SimClient::onstarted);
    }
    for(int i=0;i<clients.size();i++)
        (threads+i)->start();
}
void test2();//N个机器人自动重建重建，服务器服务器人数N和处理耗时的关系
void test3();//消息长度和时间的处理时间的关系

#endif // MYTEST_H
