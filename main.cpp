#include <QCoreApplication>
#include <stdlib.h>
#include <QMutex>
#include <QDateTime>
#include <QFile>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include "manageserver.h"
//传入的apo需要重新保存，使得n按顺序
#include "basicdatamanage.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <iostream>
#include <signal.h>
#include <messageserver.h>

#include <QSqlDatabase>

#include <QMap>
#include <QtAlgorithms>
//传入的apo需要重新保存，使得n按顺序
QString vaa3dPath;
QMap<QString,QStringList> m_MapImageIdWithRes;
QMap<QString,QString> m_MapImageIdWithDir;
QSqlDatabase globalDB;
QFile file("log.txt");

QString databaseName="Hi5";
QString dbHostName="localhost";
QString dbUserName="hi5";
QString dbPassword="!helloHi5";
QString filedir="/Users/huanglei/Desktop/swc2";

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);
void processImageSrc();
void main_my_type_user(QString);
void main_my_heat(QString);
void signalhandle(int )
{
    auto servers=Map::NeuronMapMessageServer.values();
    for(auto s:servers)
        s->autosave();
}
void head1(QString filename);
void doVersion(QString);
void doVersionNT(QString);
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

   auto filenames=QDir(filedir).entryInfoList(QDir::Files);

   for(auto &file:filenames)
   {
//        doVersion(filedir+"/"+file.baseName());
//       head1(filedir+"/"+file.baseName());
        doVersionNT(filedir+"/"+file.baseName());
   }
    exit(0);
    return a.exec();
}
bool isOther(const NeuronSWC &n,const NeuronTree &nt,int s)
{
    for(auto node:nt.listNeuron)
    {
        if(
          int(node.r)/10!=int(n.r)/10
        &&node.x>=n.x-s&&node.x<=n.x+s
        &&node.y>=n.y-s&&node.y<=n.y+s
        &&node.z>=n.z-s&&node.z<=n.z+s
                )
            return true;
    }
    return false;
}
void head1(QString filename)
{
    auto nt=readSWC_file(filename+".eswc");
    int s=32;
    for(auto &node:nt.listNeuron)
    {
        node.type=isOther(node,nt,s)?2:3;//有其他人->2号色
    }
    writeESWC_file(filename+QString("__%1.eswc").arg(s),nt);
}

bool compare(V_NeuronSWC seg1,V_NeuronSWC seg2)
{
    return seg1.row[0].timestamp<seg2.row[0].timestamp;
}

void doVersionNT(QString filename)
{
    auto nt=readSWC_file(filename+".eswc");
    auto nts=NeuronTree__2__V_NeuronSWC_list(nt);
    sort(nts.seg.begin(),nts.seg.end(),compare);
    int cnt=nts.seg.size();
    for(auto p:{0.125,0.25,0.4,0.6,1.0})
    {
        V_NeuronSWC_list nt_;
        for(int i=0;i<p*cnt;i++)
            nt_.seg.push_back(nts.seg[i]);
       writeESWC_file(filename+QString("_%1.eswc").arg(p),V_NeuronSWC_list__2__NeuronTree(nt_));
    }
}

void doVersion(QString filename)
{
    QStringList ods;

    QFile f(filename+".txt");
    f.open(QIODevice::ReadOnly|QIODevice::Text);
   while(!f.atEnd())
   {
       auto ss=f.readLine().trimmed();
       ods.push_back(ss.right(ss.size()-22));
   }
    int k=0;
    for(auto p:{0.125,0.25,0.375,0.625,1.0})
    {
        MessageServer s;
        k=ods.size()*p;
        for(int i=0;i<k;i++)
        {
            QRegExp msgreg("/(.*)_(.*):(.*)");
            if(msgreg.indexIn(ods[i])!=-1)
            {


                QString operationtype=msgreg.cap(1).trimmed();
    //                bool isNorm=msgreg.cap(2).trimmed()=="norm";
                QString operatorMsg=msgreg.cap(3).trimmed();
                if(operationtype == "drawline" )
                {
                    s.drawline(operatorMsg);
                }
                else if(operationtype == "delline")
                {
                    s.delline(operatorMsg);
                }
                else if(operationtype == "addmarker")
                {
                   s.addmarker(operatorMsg);
                }
                else if(operationtype == "delmarker")
                {
                   s.delmarekr(operatorMsg);
                }
                else if(operationtype == "retypeline")
                {
                   s.retypeline(operatorMsg);
                }
            }
        }
        writeESWC_file(filename+QString("_%1.eswc").arg(p),V_NeuronSWC_list__2__NeuronTree(s.segments));
    }

}

void processImageSrc()
{
    m_MapImageIdWithDir.clear();
    m_MapImageIdWithRes.clear();
    QFile data(QCoreApplication::applicationDirPath()+"/imageSrc.txt");
    if (data.open(QFile::ReadOnly)) {
        QTextStream in(&data);
        while (!in.atEnd()) {
            QString imageId;
            QString imageName;
            int resCnt;
            in>>imageId>>imageName>>resCnt;
            m_MapImageIdWithDir.insert(imageId,imageName);
            QStringList list;
            for(int i=0;i<resCnt;i++)
            {
                in>>imageName;
                list.push_back(imageName);
            }
            m_MapImageIdWithRes.insert(imageId,list);
        }
    }
    data.close();
}


void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // 加锁
    static QMutex mutex;
    mutex.lock();
    QByteArray localMsg = msg.toLocal8Bit();
    // 设置输出信息格式
    QString strDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss : ");
    QString strMessage = QString("DateTime:%1 %2\n")
            .arg(strDateTime)/*.arg(context.file).arg(context.line).arg(context.function)*/.arg(localMsg.constData());
// File:%2  Line:%3  Function:%4\n
    QTextStream stream(&file);
    stream << strMessage ;
    file.flush();
    // 解锁

    fprintf(stderr,strMessage.toStdString().c_str());
    mutex.unlock();
}

const int types[] = {3,4,5,6,7,8,9,10};
void main_my_type_user(QString filename)
{
    auto nt=readSWC_file(filename);
    {
        auto nt_usertype=nt;
        QMap<int,QVector<V_NeuronSWC>> map_user_nts,map_type_nts;
        QMap<int,int> map_user_type;

        for(auto &node:nt_usertype.listNeuron)
        {
            node.type=int(node.radius)/10;
        }
        auto segs=NeuronTree__2__V_NeuronSWC_list((nt_usertype));
        int usercnt=0;
        for(auto seg:segs.seg)
        {
            map_user_nts[seg.row.at(0).type].push_back(seg);
        }
        for(auto i:map_user_nts.keys())
        {
            map_user_type[i]=types[usercnt++];
        }

        for(auto &node:nt_usertype.listNeuron)
        {
            node.type=map_user_type[node.type];
        }
        for(auto i:map_user_nts.keys())
        {
            map_type_nts[map_user_type.value(i)]=map_user_nts.value(i);
        }
        //nt has been typed by user,and map_type_nt type-nts
        QMap<int,double> map_type_length;
        for(auto type:map_type_nts.keys())
        {
            auto segs=map_type_nts.value(type);
            double sum=0;
            for(auto seg:segs)
            {
                if(seg.row.size()==1) continue;
                for(int i=1;i<seg.row.size();i++)
                {
                    sum+=sqrt(pow(seg.row[i].x-seg.row[i-1].x,2)+pow(seg.row[i].y-seg.row[i-1].y,2)+pow(seg.row[i].z-seg.row[i-1].z,2));
                }
            }
            map_type_length[type]=sum;
        }
        qDebug()<<"map_type_length:\n"<<map_type_length.keys()<<endl<<map_type_length.values();

        writeESWC_file(filename.chopped(5)+"_user.eswc",nt_usertype);
//        double sum=0;
//        for(auto seg:segs.seg)
//        {
//            if(seg.row.size()==1) continue;
//            for(int i=1;i<seg.row.size();i++)
//            {
//                sum+=sqrt(pow(seg.row[i].x-seg.row[i-1].x,2)+pow(seg.row[i].y-seg.row[i-1].y,2)+pow(seg.row[i].z-seg.row[i-1].z,2));
//            }
//        }
//        for(auto d:map_type_length.values())
//               sum-=d;
//        qDebug()<<sum;
    }
}

int accHeat(const NeuronTree &nt,double x,double y,double z,int user,double half_length)
{
    int cnt=0;
    QSet<int> vs;
    for(const auto &node:nt.listNeuron)
    {
        if(
         node.x>=x-half_length&&node.x<=x+half_length
         &&node.y>=y-half_length&&node.y<=y+half_length
         &&node.z>=z-half_length&&node.z<=z+half_length)
        {
            vs.insert(node.radius/10);
        }
    }
    return  vs.size();
}

void main_my_heat(QString filename)
{
    auto nt=readSWC_file(filename);
    for(auto &node:nt.listNeuron)
    {
        node.type=accHeat(nt,node.x,node.y,node.z,node.radius/10,32);
    }
    {
        auto t=nt;
        for(auto &node:t.listNeuron)
            node.type=types[node.type];
        writeESWC_file(filename.chopped(5)+"_pointheat.eswc",t);
    }

    QMap<int,QVector<V_NeuronSWC>> temp;
    auto nts=NeuronTree__2__V_NeuronSWC_list(nt);
    QSet<int> tt;
    for(auto &seg:nts.seg)
    {
        int max=1;
        for(int i=0;i<seg.row.size();i++)
        {
            max=seg.row[i].type>max?seg.row[i].type:max;
        }
        for(int i=0;i<seg.row.size();i++)
        {
            seg.row[i].type=max;
        }
        temp[max].push_back(seg);

        tt.insert(max);
    }
    nt=V_NeuronSWC_list__2__NeuronTree(nts);
    qDebug()<<tt;
    for(auto &node:nt.listNeuron)
        node.type=types[node.type];
    writeESWC_file(filename.chopped(5)+"_lineheat.eswc",nt);
//    QMap<int,double> map_type_length;
//    for(auto type:temp.keys())
//    {
//        auto segs=temp.value(type);
//        double sum=0;
//        for(auto seg:segs)
//        {
//            if(seg.row.size()==1) continue;
//            for(int i=1;i<seg.row.size();i++)
//            {
//                sum+=sqrt(pow(seg.row[i].x-seg.row[i-1].x,2)+pow(seg.row[i].y-seg.row[i-1].y,2)+pow(seg.row[i].z-seg.row[i-1].z,2));
//            }
//        }
//        map_type_length[type]=sum;
//    }
//    qDebug()<<map_type_length;
//    double sum=0;
//    auto segs=NeuronTree__2__V_NeuronSWC_list(nt);
//    for(auto seg:segs.seg)
//    {
//        if(seg.row.size()==1) continue;
//        for(int i=1;i<seg.row.size();i++)
//        {
//            sum+=sqrt(pow(seg.row[i].x-seg.row[i-1].x,2)+pow(seg.row[i].y-seg.row[i-1].y,2)+pow(seg.row[i].z-seg.row[i-1].z,2));
//        }
//    }
//    for(auto d:map_type_length.values())
//           sum-=d;
//    qDebug()<<sum;

}
