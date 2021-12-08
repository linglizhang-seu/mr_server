#ifndef ANALYSELOG_H
#define ANALYSELOG_H

#include <QFile>
#include <QDateTime>
#include "utils.h"
#include <algorithm>
#include <cmath>
#include <QRegExp>

QStringList readorders(QString infile)
{
    const int thirdId=20;
    QFile f(infile);
    if(!f.open(QIODevice::ReadOnly)){
        return {};
    }

    auto orders=f.readAll().split('\n');
    QStringList res;
    for(auto array:orders)
        res.push_back(QString(array));
    return res;
}

std::pair<QDateTime,QString> getMsgwithTime(const QString &msg)
{
    QString timestr=msg.left(19);
    QString data=msg.right(msg.size()-22);
    return {QDateTime::fromString(timestr,"yyyy/MM/dd hh:mm:ss"),data};
}

V_NeuronSWC_list doorders(QStringList orders)
{
    V_NeuronSWC_list segments;
    QList<CellAPO> wholePoints;
    QStringList stack;
    for(auto &msg:orders){
        auto pair=getMsgwithTime(msg);
        QRegExp msgreg("/(.*)_(.*):(.*)");
        if(msgreg.indexIn(pair.second)!=-1)
        {
            QString operationtype=msgreg.cap(1).trimmed();
//                bool isNorm=msgreg.cap(2).trimmed()=="norm";
            QString operatorMsg=msgreg.cap(3).trimmed();
            if(operationtype == "drawline" )
            {
                drawline(operatorMsg,segments);
            }
            else if(operationtype == "delline")
            {
                 if(delline(operatorMsg,segments))
                     stack.push_back(operatorMsg);
            }
            else if(operationtype == "addmarker")
            {
                addmarker(operatorMsg,wholePoints);
            }
            else if(operationtype == "delmarker")
            {
                delmarekr(operatorMsg,wholePoints);
            }
            else if(operationtype == "retypeline")
            {
                retypeline(operatorMsg,segments);
            }
        }
    }
    return segments;
}

void getUnUse(QString inlog,QString outswc)
{
    auto orders=readorders(inlog);
    V_NeuronSWC_list segments;
    QList<CellAPO> wholePoints;
    QStringList stack;
    for(auto &msg:orders){
        auto pair=getMsgwithTime(msg);
        QRegExp msgreg("/(.*)_(.*):(.*)");
        if(msgreg.indexIn(pair.second)!=-1)
        {
            QString operationtype=msgreg.cap(1).trimmed();
//                bool isNorm=msgreg.cap(2).trimmed()=="norm";
            QString operatorMsg=msgreg.cap(3).trimmed();
            if(operationtype == "drawline" )
            {
                drawline(operatorMsg,segments);
            }
            else if(operationtype == "delline")
            {
                 if(delline(operatorMsg,segments))
                     stack.push_back(operatorMsg);
            }
            else if(operationtype == "addmarker")
            {
                addmarker(operatorMsg,wholePoints);
            }
            else if(operationtype == "delmarker")
            {
                delmarekr(operatorMsg,wholePoints);
            }
            else if(operationtype == "retypeline")
            {
                retypeline(operatorMsg,segments);
            }
        }
    }
//    writeESWC_file(outswc,V_NeuronSWC_list__2__NeuronTree(segments));
    for(auto &msg:stack){
        drawlineWithThirdParty(msg,segments);
    }
    writeESWC_file(outswc,V_NeuronSWC_list__2__NeuronTree(segments));
}

std::pair<double,double> doThirdPartys(QString inlog)
{
    auto orders=readorders(inlog);
    double thirdlen=0;
    QStringList stack;
    for(auto &msg:orders){
        auto pair=getMsgwithTime(msg);
        QRegExp msgreg("/(.*)_(.*):(.*)");
        if(msgreg.indexIn(pair.second)!=-1)
        {
            QString operationtype=msgreg.cap(1).trimmed();
            QString operatorMsg=msgreg.cap(3).trimmed();
            if(operationtype=="drawline"||operationtype=="delline")
            {
                QStringList listwithheader=operatorMsg.split(';',Qt::SkipEmptyParts);
                if(listwithheader.size()<=1)
                {
                    qDebug()<<"msg only contains header:"<<pair.second;
                    continue;
                }

                int from=0;
                QString username;

                {
                    auto headerlist=listwithheader[0].split(' ',Qt::SkipEmptyParts);
                    QString clienttype=headerlist[1].trimmed();
                    for(int i=0;i<clienttypes.size();i++)
                    {
                        if(clienttypes[i]==clienttype)
                        {
                            from = i;
                            break;
                        }
                    }
                    username=headerlist[0].trimmed();
                }
                if(getid(username)==20)
                {
                    stack.push_back(msg);
                    auto nt=convertMsg2NT(listwithheader,username,from);
                    auto seg=NeuronTree__2__V_NeuronSWC_list(nt).seg[0];
                    thirdlen+=getsegmentlength(seg);
                }
            }
        }
    }

    auto segs=doorders(stack);
    double len=0.0;
    for(auto &seg:segs.seg){
        len+=getsegmentlength(seg);
    }
    return {thirdlen,len};
}

void getThirdValues(QStringList files,QString file)
{
    QMap<QString,std::pair<double,double>> hashmap;//文件名 变动长度 结果长度
    for(auto filr:files)
    {
        hashmap[filr]=doThirdPartys(filr);
    }
//    double average=std::accumulate(values.begin(),values.end(),0.0  )*1.0/values.size();
//    double var=0;
//    for(int j=0;j<values.size();j++)
//        var+=pow(values[j]-average,2);
//    var/=values.size();
    QFile f(file);
    if(f.open(QIODevice::WriteOnly)){
//        QString avestr=QString("average:%1\n").arg(average);
//        QString varstr=QString("var=%1\n").arg(var);
//        f.write(avestr.toStdString().c_str(),avestr.size());
//        f.write(varstr.toStdString().c_str(),varstr.size());

        QString data=QString("%1:%2:%3\n").arg("文件名").arg("变动长度").arg("有效变动长度");
        f.write(data.toStdString().c_str(),data.size());
        for(int i=0;i<files.size();i++){
            QString data=QString("%1:%2:%3\n").arg(files[i]).arg(hashmap[files[i]].first).arg(hashmap[files[i]].second);
            f.write(data.toStdString().c_str(),data.size());
        }
        f.close();
    }
}

QMap<int,int> getusertimes(QString file)
{
     QMap<int,QList<QDateTime>> usertimes;

     auto orders=readorders(file);
     V_NeuronSWC_list segments;
     QList<CellAPO> wholePoints;

     for(auto &msg:orders){
         auto pair=getMsgwithTime(msg);
         QRegExp msgreg("/(.*)_(.*):(.*)");
         if(msgreg.indexIn(pair.second)!=-1)
         {
             QString operationtype=msgreg.cap(1).trimmed();
             QString operatorMsg=msgreg.cap(3).trimmed();
             if(operationtype == "drawline" )
             {
                 drawline(operatorMsg,segments);
             }
             else if(operationtype == "delline")
             {
                  delline(operatorMsg,segments);
             }
             else if(operationtype == "addmarker")
             {
                 addmarker(operatorMsg,wholePoints);
             }
             else if(operationtype == "delmarker")
             {
                 delmarekr(operatorMsg,wholePoints);
             }
             else if(operationtype == "retypeline")
             {
                 retypeline(operatorMsg,segments);
             }

             {
                 QStringList listwithheader=operatorMsg.split(';',Qt::SkipEmptyParts);
                 if(listwithheader.size()<=1)
                 {
                     qDebug()<<"msg only contains header:"<<pair.second;
                     continue;
                 }

                 QString username;
                 {
                     auto headerlist=listwithheader[0].split(' ',Qt::SkipEmptyParts);
                     username=headerlist[0].trimmed();
                 }
                 usertimes[username.toInt()].push_back(pair.first);
             }
         }
     }
    const int plus=60*2;//时间补偿
    const int thres=60*3;//gap thres
     QMap<int,int> userinfos;
     auto keys=usertimes.keys();
     for(auto &key:keys){
         QList<QDateTime> times=usertimes.value(key);

         int cnt=times.size();
         if(cnt==0) {
             userinfos[key+2]=0;
         }else if(cnt==1){
             userinfos[key+2]=plus;
         }else{
             userinfos[key+2]=times.back().toSecsSinceEpoch()-times.front().toSecsSinceEpoch()+plus;
             for(int i=1;i<cnt;i++)
             {
                if(times[i].toSecsSinceEpoch()-times[i-1].toSecsSinceEpoch()>60*3)
                    userinfos[key+2]-=(times[i].toSecsSinceEpoch()-times[i-1].toSecsSinceEpoch()-plus);
             }
         }
     }
     return userinfos;
}

void getspeed(QString inlogfile,QString inswc,QString outfile)
{
    auto times=getusertimes(inlogfile);
    auto nt=readSWC_file(inswc);
    for(auto &node:nt.listNeuron){
        node.type=node.r/10+2;
    }
    auto segments=NeuronTree__2__V_NeuronSWC_list(nt);

    QMap<int,double> hashmap;
    for(auto &seg:segments.seg){
        hashmap[int(seg.row[0].type)]+=getsegmentlength(seg);
    }

    QMap<int,double> speeds;
    if(times.keys().size()!=hashmap.keys().size()){
        qDebug()<<"Error";
        return;
    }
    auto keys=hashmap.keys();
    for(auto key:keys){
        speeds[key]=hashmap.value(key)/times.value(key);
    }
    QFile f(outfile);
    if(f.open(QIODevice::WriteOnly)){
        QString data=QString("%1:%2 %3 %4\n").arg("usertype").arg("time").arg("length").arg("speed");
        f.write(data.toStdString().c_str(),data.size());
        for(auto key:keys){
            QString data=QString("%1:%2 %3 %4\n").arg(key).arg(times.value(key)).arg(hashmap.value(key)).arg(speeds.value(key));
            f.write(data.toStdString().c_str(),data.size());
        }
        f.close();
    }
}

void verifylog(QString infile)
{
         auto orders=readorders(infile);
         for(int i=0;i<orders.size();i++){
             auto msg=orders[i];
             auto pair=getMsgwithTime(msg);
             QRegExp msgreg("/(.*)_(.*):(.*)");
             if(msgreg.indexIn(pair.second)!=-1)
             {
                 QString operationtype=msgreg.cap(1).trimmed();
                 QString operatorMsg=msgreg.cap(3).trimmed();
                 if(operationtype=="drawline"){
                     QStringList listwithheader=operatorMsg.split(';',Qt::SkipEmptyParts);
                     auto data1=listwithheader[1].split(' ',Qt::SkipEmptyParts);
                     if(data1[0].toInt()!=3){
                         qDebug()<<i<<":"<<msg;
                     }
                 }
             }
         }
}




#endif // ANALYSELOG_H
