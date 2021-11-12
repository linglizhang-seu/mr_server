#include <QCoreApplication>
#include <stdlib.h>
#include <QMutex>
#include <QDateTime>
#include <QFile>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <basic_c_fun/basic_surf_objs.h>
#include <basic_c_fun/neuron_format_converter.h>
#include "simclient.h"

//传入的apo需要重新保存，使得n按顺序
QString port="4167";
int peopleCnt=20;
int packageCnt=10;//MESSGE CNOUT
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


QStringList V_NeuronSWCToSendMSG(V_NeuronSWC seg)
{
    QStringList result;
    for(int i=0;i<seg.row.size();i++)   //why  i need  < 120, does msg has length limitation? liqi 2019/10/7
    {
        V_NeuronSWC_unit curSWCunit = seg.row[i];
        result.push_back(QString("%1 %2 %3 %4").arg(curSWCunit.type).arg(curSWCunit.x).arg(curSWCunit.y).arg(curSWCunit.z));
    }
    return result;
}

QString UpdateAddSegMsg(V_NeuronSWC seg,QString clienttype,int i)
{
    QStringList result;
    result.push_back(QString("%1 %2 %3 %4 %5").arg(i).arg(clienttype).arg(10000).arg(10000).arg(10000));
    result+=V_NeuronSWCToSendMSG(seg);
    return "/drawline_norm:"+result.join(';');
}

QString UpdateDelSegMsg(V_NeuronSWC seg,QString clienttype,int i)
{
    QStringList result;
    result.push_back(QString("%1 %2 %3 %4 %5").arg(i).arg(clienttype).arg(10000).arg(10000).arg(10000));
    result+=V_NeuronSWCToSendMSG(seg);
    return QString("/delline_norm:"+result.join(";"));

}

QString UpdateAddMarkerMsg(float X, float Y, float Z,int type,QString clienttype,int i)
{
    QStringList result;
    result.push_back(QString("%1 %2 %3 %4 %5").arg(i).arg(clienttype).arg(10000).arg(10000).arg(10000));
    result.push_back(QString("%1 %2 %3 %4").arg(type).arg(X).arg(Y).arg(Z));
    return QString("/addmarker_norm:"+result.join(";"));
}

QString UpdateDelMarkerSeg(float x,float y,float z,QString clienttype,int i)
{
    QStringList result;
    result.push_back(QString("%1 %2 %3 %4 %5").arg(i).arg(clienttype).arg(10000).arg(10000).arg(10000));
    result.push_back(QString("%1 %2 %3 %4").arg(-1).arg(x).arg(y).arg(z));
    return QString("/delmarker_norm:"+result.join(";"));
}

QString UpdateRetypeSegMsg(V_NeuronSWC seg,int type,QString clienttype,int i)
{
    QStringList result;
    result.push_back(QString("%1 %2 %3 %4 %5 %6").arg(i).arg(clienttype).arg(type).arg(10000).arg(10000).arg(10000));
    result+=V_NeuronSWCToSendMSG(seg);
    return QString("/retypeline_norm:"+result.join(";"));
}

QList<QStringList> MsgForAddMarker(NeuronTree nt)
{
    int cnt=2;
    const int cntForPeople=cnt*packageCnt;
    std::shuffle(nt.listNeuron.begin(),nt.listNeuron.end(),std::default_random_engine());
    QList<NeuronSWC> markers(nt.listNeuron.begin(),nt.listNeuron.end()+peopleCnt*cntForPeople);
    QList<QStringList> res;
    for(int i=0;i<peopleCnt;i++)
    {
        QStringList tmp;
        for(int j=0;j<cntForPeople;j++){
            int index=cntForPeople*i+j;
            tmp.push_back(UpdateAddMarkerMsg(markers[index].x,markers[index].y,markers[index].z,i%10+2,"TeraVR",i));
        }
        res.push_back(tmp);
    }
    return res;

}

QList<QStringList> MsgForDeleteMarker(NeuronTree nt)
{
    int cnt=1;
    const int cntForPeople=cnt*packageCnt;
    std::shuffle(nt.listNeuron.begin(),nt.listNeuron.end(),std::default_random_engine());
    QList<NeuronSWC> markers(nt.listNeuron.begin(),nt.listNeuron.end()+cnt*peopleCnt*packageCnt);
    QList<QStringList> res;
    for(int i=0;i<peopleCnt;i++)
    {
        QStringList tmp;
        for(int j=0;j<cntForPeople;j++){
            int index=cntForPeople*i+j;
            tmp.push_back(UpdateDelMarkerSeg(markers[index].x,markers[index].y,markers[index].z,"TeraVR",i));
        }
        res.push_back(tmp);
    }
    return res;
}

QList<QStringList> MsgForAddLine(V_NeuronSWC_list nt)
{
    int cnt=1;//每个包加线命令的条数
    const int cntForPeople=cnt*packageCnt;
    QList<QList<V_NeuronSWC>> segments; //每个人的线集合
    for(int i=0;i<peopleCnt;i++){
        std::shuffle(nt.seg.begin(),nt.seg.end(),std::default_random_engine());
        segments.push_back(QList<V_NeuronSWC>(nt.seg.begin(),nt.seg.begin()+cntForPeople));
    }

    QList<QStringList> res;//每个人的线集合->每个人的消息集合
    for(int i=0;i<peopleCnt;i++){
        QStringList tmp;
        for(int j=0;j<cntForPeople;j++){
            tmp.append(UpdateAddSegMsg(segments[i][j],"TeraVR",i));
        }
        res.append(tmp);
    }
    return res;
}

QList<QStringList> MsgForDeleteLine(V_NeuronSWC_list nt)
{
    int cnt=3;//每个包里减线命令的条数
    const int cntForPeople=cnt*packageCnt;
    QList<QList<V_NeuronSWC>> segments;
    for(int i=0;i<peopleCnt;i++){
        std::shuffle(nt.seg.begin(),nt.seg.end(),std::default_random_engine());
        segments.push_back(QList<V_NeuronSWC>(nt.seg.begin(),nt.seg.begin()+cntForPeople));
    }
    QList<QStringList> res;
    for(int i=0;i<peopleCnt;i++){
        QStringList tmp;
        for(int j=0;j<cntForPeople;j++){
            tmp.append(UpdateDelSegMsg(segments[i][j],"TeraVR",i));
        }
        res.append(tmp);
    }
    return res;
}

QList<QStringList> MsgForRetypeLine(V_NeuronSWC_list nt)
{
    int cnt=2;
    const int cntForPeople=cnt*packageCnt;
    QList<QList<V_NeuronSWC>> segments;
    for(int i=0;i<peopleCnt;i++){
        std::shuffle(nt.seg.begin(),nt.seg.end(),std::default_random_engine());
        segments.push_back(QList<V_NeuronSWC>(nt.seg.begin(),nt.seg.begin()+cntForPeople));
    }
    QList<QStringList> res;
    for(int i=0;i<peopleCnt;i++){
        QStringList tmp;
        for(int j=0;j<cntForPeople;j++){
            tmp.append(UpdateRetypeSegMsg(segments[i][j],i%10+2,"TeraVR",i));
        }
        res.append(tmp);
    }
    return res;
}


QList<QStringList> MsgWaitSend(QList<QStringList> addline,
                               QList<QStringList> delline,
                               QList<QStringList> retypeline,
                               QList<QStringList> addmarker,
                               QList<QStringList> deletemarker)

{
    QList<QStringList> res;//每个人的操作集合
    for(int i=0;i<peopleCnt;i++){
        QStringList msgs=addline[i]+delline[i]+retypeline[i]+addmarker[i]+deletemarker[i];
        std::shuffle(msgs.begin(),msgs.end(),std::default_random_engine());
        res.push_back(msgs);
    }
    return res;
}

QList<QStringList> prepareMsg(NeuronTree nt)
{
    auto segments=NeuronTree__2__V_NeuronSWC_list(nt);

    auto addline=MsgForAddLine(segments);
//    auto delline=MsgForDeleteLine(segments);
//    auto retypeline=MsgForRetypeLine(segments);
//    auto addmarker=MsgForAddMarker(nt);
//    auto delmarker=MsgForDeleteMarker(nt);

    return MsgWaitSend(addline,{},{},{},{});
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageOutput);
    QCoreApplication a(argc, argv);
    auto nt=readSWC_file("/Users/huanglei/Desktop/2.eswc");
    auto msgLists=prepareMsg(nt);
    QString ip="139.155.28.154";


    QThread *threads=new QThread[peopleCnt];
    QVector<SimClient*> clients;
    for(int i=0;i<peopleCnt;i++){
        auto p=new SimClient(ip,port,QString::number(i),msgLists[i]);
        clients.push_back(p);
        QObject::connect(threads+i,SIGNAL(started()),p,SLOT(onstarted()));
        clients[i]->moveToThread(threads+i);
    }
    for(int i=0;i<peopleCnt;i++)
        threads[i].start();

    return a.exec();
}

