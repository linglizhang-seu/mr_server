#include <QCoreApplication>
#include <stdlib.h>
#include <QMutex>
#include <QDateTime>
#include <QFile>
#include <cstdlib>
#include <cstdio>
#include <basic_c_fun/basic_surf_objs.h>
#include <basic_c_fun/neuron_format_converter.h>
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

int peopleCnt=10;
int packageCnt=1;
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

QList<QStringList> MsgForAddMarker(const NeuronTree &nt)
{
    int cnt=2;
    const int cntForPeople=cnt*packageCnt;
    random_shuffle(nt.listNeuron.begin(),nt.listNeuron.end());
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

QList<QStringList> MsgForDeleteMarker(const NeuronTree &nt)
{
    int cnt=1;
    const int cntForPeople=cnt*packageCnt;
    random_shuffle(nt.listNeuron.begin(),nt.listNeuron.end());
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
    int cnt=10;
    const int cntForPeople=cnt*packageCnt;
    QList<QList<V_NeuronSWC>> segments;
    for(int i=0;i<peopleCnt;i++){
        random_shuffle(nt.seg.begin(),nt.seg.end());
        segments.push_back(QList<V_NeuronSWC>(nt.seg.begin(),nt.seg.begin()+cntForPeople));
    }
    QList<QStringList> res;
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
    int cnt=3;
    const int cntForPeople=cnt*packageCnt;
    QList<QList<V_NeuronSWC>> segments;
    for(int i=0;i<peopleCnt;i++){
        random_shuffle(nt.seg.begin(),nt.seg.end());
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
        random_shuffle(nt.seg.begin(),nt.seg.end());
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
                               QList<QStringList> splitline,
                               QList<QStringList> addmarker,
                               QList<QStringList> deletemarker)

{
    QList<QStringList> res;
    for(int i=0;i<peopleCnt;i++){
        QStringList msgs=addline[i]+delline[i]+retypeline[i]+addmarker[i]+deletemarker[i];
        random_shuffle(msgs.begin(),msgs.end());
        res.push_back(msgs);
    }
    return res;
}

QList<QStringList> prepareMsg(NeuronTree nt)
{
    auto segments=NeuronTree__2__V_NeuronSWC_list(nt);

    auto addline=MsgForAddLine(segments);
    auto delline=MsgForDeleteLine(segments);
    auto retypeline=MsgForRetypeLine(segments);
    auto addmarker=MsgForAddMarker(nt);
    auto delmarker=MsgForDeleteMarker(nt);

    return MsgWaitSend(addline,delline,retypeline,addmarker,delmarker);
}




int main(int argc, char *argv[])
{

    auto nt=readSWC_file("/Users/huanglei/Desktop/prefanceTest.eswc");
    qDebug()<<nt.listNeuron.size();
    qDebug()<<NeuronTree__2__V_NeuronSWC_list(nt).seg.size();

    random_shuffle(nt.listNeuron.begin(),nt.listNeuron.end());
    int markerCnt=2;
    int peopleCnt=10;
    int packageCnt=1;
    QList<NeuronSWC> markers(nt.listNeuron.begin(),nt.listNeuron.begin()+markerCnt*peopleCnt*packageCnt);




    qInstallMessageHandler(myMessageOutput);
    QCoreApplication a(argc, argv);

    ManageServer server;
    if(!server.listen(QHostAddress::Any,26371))
    {
        qDebug()<<"Error:cannot start server in port 9999,please check!";
        exit(-1);
    }else
    {
        qDebug()<<"server(2.0.4.1) for vr_farm started!";
    }
    return a.exec();
}

