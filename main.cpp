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
#include <iostream>
#include <QFile>
#include <QTime>
//传入的apo需要重新保存，使得n按顺序
QString port="4001";
int peopleCnt=10;//peopleCnt
int packageCnt=1;//MESSGE CNOUT
const int threadCnt=10;//peopleCnt/2
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
    QFile file("/Users/huanglei/Desktop/log.txt");
    if(!file.open(QIODevice::ReadWrite | QIODevice::Append))
        qDebug()<<file.errorString();
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
    result.push_back(QString("%1 %2 %3 %4 %5").arg(clienttype).arg(i).arg(10000).arg(10000).arg(10000));
    result+=V_NeuronSWCToSendMSG(seg);
    return "/drawline_norm:"+result.join(',');
}

QString UpdateDelSegMsg(V_NeuronSWC seg,QString clienttype,int i)
{
    QStringList result;
    result.push_back(QString("%1 %2 %3 %4 %5").arg(i).arg(clienttype).arg(10000).arg(10000).arg(10000));
    result+=V_NeuronSWCToSendMSG(seg);
    return QString("/delline_norm:"+result.join(","));

}

QString UpdateAddMarkerMsg(float X, float Y, float Z,int type,QString clienttype,int i)
{
    QStringList result;
    result.push_back(QString("%1 %2 %3 %4 %5").arg(clienttype).arg(i).arg(10000).arg(10000).arg(10000));
    result.push_back(QString("%1 %2 %3 %4").arg(type).arg(X).arg(Y).arg(Z));
    return QString("/addmarker_norm:"+result.join(","));
}

QString UpdateDelMarkerSeg(float x,float y,float z,QString clienttype,int i)
{
    QStringList result;
    result.push_back(QString("%1 %2 %3 %4 %5").arg(clienttype).arg(i).arg(10000).arg(10000).arg(10000));
    result.push_back(QString("%1 %2 %3 %4").arg(-1).arg(x).arg(y).arg(z));
    return QString("/delmarker_norm:"+result.join(","));
}

QString UpdateRetypeSegMsg(V_NeuronSWC seg,int type,QString clienttype,int i)
{
    QStringList result;
    result.push_back(QString("%1 %2 %3 %4 %5 %6").arg(clienttype).arg(i).arg(type).arg(10000).arg(10000).arg(10000));
    result+=V_NeuronSWCToSendMSG(seg);
    return QString("/retypeline_norm:"+result.join(","));
}

QList<QStringList> MsgForAddMarker(NeuronTree nt)
{
    int cnt=1;
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
    int cnt=1;//每个包里减线命令的条数
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
    int cnt=1;
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
        QStringList msgs=addline[i]/*+delline[i]+retypeline[i]+addmarker[i]+deletemarker[i];*/;
//        QStringList msgs=addline[i]+delline[i]+retypeline[i]+addmarker[i]+deletemarker[i];
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

//    return MsgWaitSend(addline,delline,retypeline,addmarker,delmarker);
    return MsgWaitSend(addline,{},{},{},{});
}


QString preProcess(QString infilepath,QString outfilepath){
    QString order=QString("cat %1 > %2").arg(infilepath).arg(outfilepath);
    system(order.toStdString().c_str());
    return outfilepath;
}

vector<double> cacPerMsgDelay(QString infilepath,QString basename){
    QFile file(infilepath);
    if(!file.open(QIODevice::ReadOnly)){
        qDebug()<<file.errorString();
    }
    QStringList paras=basename.split('_');

    QStringList list=QString(file.readAll()).split('\n',Qt::SkipEmptyParts);
    file.close();
    if(list.size()!=paras[3].toUInt()*paras[4].toUInt()){
        qDebug()<<"Error:"+infilepath;
    }

    QDateTime start=QDateTime::fromString(list[0].left(23),"yyyy/MM/dd hh:mm:ss.zzz");
    QDateTime end=QDateTime::fromString(list.back().left(23),"yyyy/MM/dd hh:mm:ss.zzz");
    qDebug()<<paras[3].toDouble()<<paras[4].toDouble()<<(end.toMSecsSinceEpoch()-start.toMSecsSinceEpoch())*1.0/list.size();
    return {paras[3].toDouble(),paras[4].toDouble(),(end.toMSecsSinceEpoch()-start.toMSecsSinceEpoch())*1.0/list.size()};
}

void processData(QString dirName){
    QDir dir(dirName);
    dir.rmdir("processed");
    dir.mkdir("processed");
    auto filelist=dir.entryInfoList(QDir::Files,QDir::Name);

    for(auto file:filelist){
        preProcess(file.absoluteFilePath(),dirName+"/processed/"+file.fileName());
    }

    dir.cd("processed");
    filelist=dir.entryInfoList(QDir::Files);
    vector<vector<double>> table;
    for(auto file:filelist){
        table.push_back(cacPerMsgDelay(file.absoluteFilePath(),file.baseName()));
    }

    QFile file(dirName+"/report");
    if(!file.open(QIODevice::WriteOnly)){
        qDebug()<<file.errorString();
    }

    QTextStream steam(&file);
    steam<<"UserCount\tMessageCount\tDelayPerMessgae\n";
    for(auto row:table){
        steam<<row[0]<<"\t"<<row[1]<<"\t"<<row[2]<<"\n";
    }
    file.close();


}


int main(int argc, char *argv[])
{
//    qInstallMessageHandler(myMessageOutput);
    QCoreApplication a(argc, argv);

    processData("/Users/huanglei/Desktop/orders");
//    processData("/Users/huanglei/Desktop/burst");
//    processData("/Users/huanglei/Desktop/log");
//    auto v=87;
//    peopleCnt*=v;
//    port=QString::number(4000+v);
//    auto nt=readSWC_file("/Users/huanglei/Desktop/2.eswc");
//    auto msgLists=prepareMsg(nt);

//    QString ip="127.0.0.1";

//    QThread *threads=new QThread[threadCnt];
//    QVector<SimClient*> clients;
//    for(int i=0;i<peopleCnt;i++){
//        auto p=new SimClient(ip,port,QString::number(i),msgLists[i]);
//        clients.push_back(p);
//        QObject::connect(threads+i%threadCnt,SIGNAL(started()),p,SLOT(onstarted()));
//        clients[i]->moveToThread(threads+(peopleCnt%threadCnt));
//    }
//    for(int i=0;i<threadCnt;i++)
//        threads[i].start();

//    return a.exec();
}
