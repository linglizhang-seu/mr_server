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

//    QString timestr=msg.left(19);
    QString timestr=msg.left(19);
    timestr=timestr.trimmed();
    qDebug()<<"timestr: "<<timestr;
    QString data=msg.right(msg.size()-22);
    qDebug()<<"data time"<<QDateTime::fromString(timestr,"yyyy/MM/dd hh:mm:ss");
    return {QDateTime::fromString(timestr,"yyyy/MM/dd hh:mm:ss"),data};
}

V_NeuronSWC_list doorders(QStringList orders)
{
    V_NeuronSWC_list segments;
    QList<CellAPO> wholePoints;
    QStringList stack;
//    int type;
    for(auto &msg:orders){
        auto pair=getMsgwithTime(msg);
        qDebug()<<"pair: "<<pair;
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
//                 if(delline(operatorMsg,segments,type))
//                     stack.push_back(operatorMsg);
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
    int type;
    QList<int> intstack;
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

//                 if(delline(operatorMsg,segments,type))
//                 {
//                     stack.push_back(operatorMsg);
//                     intstack.push_back(type);
//                     qDebug()<<"type_list"<<intstack;
//                 }

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
    double lengthuse=getsegmentslength(segments);
//    writeESWC_file(outswc,V_NeuronSWC_list__2__NeuronTree(segments));
    //lingli add
//    for(int i=0;i<stack.size();i++)
//    {
//        qDebug()<<"stack"<<stack[i];
//        qDebug()<<"intstack"<<intstack[i];
//        drawlineUnuse(stack[i],segments,intstack[i]);

//    }
    //for type
    for(auto &msg:stack){
        drawlineUnuse(msg,segments);
    }
    double lengthall=getsegmentslength(segments);
    QFile f(QString::number(lengthuse/lengthall));
    if (f.open(QIODevice::WriteOnly)){
        f.close();
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
                QStringList listwithheader=operatorMsg.split(',',Qt::SkipEmptyParts);
                if(listwithheader.size()<=1)
                {
                    qDebug()<<"msg only contains header:"<<pair.second;
                    continue;
                }

                int from=0;
                QString username;

                {
                    auto headerlist=listwithheader[0].split(' ',Qt::SkipEmptyParts);
                    QString clienttype=headerlist[0].trimmed();
                    for(int i=0;i<clienttypes.size();i++)
                    {
                        if(clienttypes[i]==clienttype)
                        {
                            from = i;
                            break;
                        }
                    }
                    username=headerlist[1].trimmed();
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

//从instruction list中抽取出每个的sub instruction list
//对于每个sub instruction list,遍历 计算前一条指令和后一条指令的时间差

std::pair<QMap<int,double>,QMap<int,double> > getusertimes(QString file)
{
     QMap<int,double> addtimes,checktimes;

     QMap<int,QStringList> hashmap;
     {
         auto orders=readorders(file);
         for(auto &msg:orders){
             auto pair=getMsgwithTime(msg);
             qDebug()<<"getusertime_pair_first"<<pair.first;
             qDebug()<<"getusertime_pair_second"<<pair.second;
             QRegExp msgreg("/(.*)_(.*):(.*)");
             if(msgreg.indexIn(pair.second)!=-1)
             {
                 QString operationtype=msgreg.cap(1).trimmed();
                 QString operatorMsg=msgreg.cap(3).trimmed();

                 QStringList listwithheader=operatorMsg.split(',',Qt::SkipEmptyParts);
                 auto headerlist=listwithheader[0].split(' ',Qt::SkipEmptyParts);
                 QString username=headerlist[1].trimmed();

                 hashmap[username.toInt()].push_back(msg);
             }
         }
     }


     const int thres=60*3;
     const int plus=30;
     auto keys=hashmap.keys();


     for(auto key:keys){
        auto instructions=hashmap.value(key);
        const int cnt=instructions.size();
        if(cnt==0) continue;
        for(int i=1;i<cnt;i++){
            QDateTime currMsgTime,preMsgTime;
            QString currOperationtype;

            {
                auto pair=getMsgwithTime(instructions[i]);
                currMsgTime=pair.first;
                qDebug()<<"curMsgTime"<<currMsgTime;
                QRegExp msgreg("/(.*)_(.*):(.*)");
                if(msgreg.indexIn(pair.second)!=-1)
                {
                   currOperationtype=msgreg.cap(1).trimmed();
                }
            }
            {
                auto pair=getMsgwithTime(instructions[i-1]);
                preMsgTime=pair.first;
            }

            int diff=currMsgTime.toSecsSinceEpoch()-preMsgTime.toSecsSinceEpoch();
            qDebug()<<"diff"<<diff;
            if(diff>thres) diff=plus;
            if(currOperationtype!="retypeline"){
                addtimes[key]+=diff;
            }else{
                checktimes[key]+=diff;
            }

        }
     }

     return {addtimes,checktimes};
}

void getspeed(QString inlogfile,QString inswc,QString outfile)
{
    auto times=getusertimes(inlogfile);
//    qDebug()<<"times_first"<<times.first;
//    qDebug()<<"times_second"<<times.second;
    auto addtimes=times.first;
    auto checktimes=times.second;

    QMap<int,double> addlengths,checklengths;
    {
        auto nt=readSWC_file(inswc);
        for(auto &node:nt.listNeuron){
            node.type=node.r/10;
        }
        auto segments=NeuronTree__2__V_NeuronSWC_list(nt);
        for(auto &seg:segments.seg){
            addlengths[int(seg.row[0].type)]+=getsegmentlength(seg);
        }
    }
    {
        auto nt=readSWC_file(inswc);
        for(auto &node:nt.listNeuron){
            node.type=node.creatmode/10;
        }
        auto segments=NeuronTree__2__V_NeuronSWC_list(nt);
        for(auto &seg:segments.seg){
            checklengths[int(seg.row[0].type)]+=getsegmentlength(seg);
        }
    }


    QMap<int,double> addspeeds,checkspeeds;
    auto keys=addtimes.keys();
    for(auto key:keys){
        addspeeds[key]=addlengths[key]/addtimes[key]*60;
        checkspeeds[key]=checklengths[key]/checktimes[key]*60;
    }

    QFile f(outfile);
    if(f.open(QIODevice::WriteOnly)){
        QString data=QString("%1:%2 %3 %4 %5 %6 %7\n").arg("usertype").arg("addtime").arg("addlength").arg("checktime").arg("checklength").arg("addspedd").arg("checkspeed");
        f.write(data.toStdString().c_str(),data.size());
        for(auto key:keys){
            QString data=QString("%1:%2 %3 %4 %5 %6 %7\n").arg(key).arg(addtimes.value(key)).arg(addlengths.value(key)).arg(checktimes.value(key)).arg(checklengths.value(key)).arg(addspeeds[key]).arg(checkspeeds[key]);
            f.write(data.toStdString().c_str(),data.size());
        }
        data=QString("Total_Add_length:%1\n").arg(std::accumulate(addlengths.begin(),addlengths.end(),0.0));
        f.write(data.toStdString().c_str(),data.size());
        data=QString("Total_Check_length:%1\n").arg(std::accumulate(checklengths.begin(),checklengths.end(),0.0));
        f.write(data.toStdString().c_str(),data.size());
        data=QString("Total_Add_time:%1\n").arg(std::accumulate(addtimes.begin(),addtimes.end(),0.0)/60);
        f.write(data.toStdString().c_str(),data.size());
        data=QString("Total_Check_time:%1\n").arg(std::accumulate(checktimes.begin(),checktimes.end(),0.0)/60);
        f.write(data.toStdString().c_str(),data.size());
        f.close();
    }
}

void dologfile(QString infile,QString out)
{
     auto orders=readorders(infile);
     QStringList stack;
//     for(int i=0;i<orders.size();i++){
//         auto msg=orders[i];
//         auto pair=getMsgwithTime(msg);
//         QRegExp msgreg("/(.*)_(.*):(.*)");
//         if(msgreg.indexIn(pair.second)!=-1)
//         {
//            stack.push_back(msgreg.cap(3).trimmed());
//         }
//     }
    auto segs=doorders(orders);
    writeESWC_file(out,V_NeuronSWC_list__2__NeuronTree(segs));
}




#endif // ANALYSELOG_H
