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
    return {QDateTime::fromString(timestr,"yyyy-MM-dd hh:mm:ss"),data};
}

void doorders(QString inlog,QString outswc)
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
    writeESWC_file(outswc,V_NeuronSWC_list__2__NeuronTree(segments));
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


int doThirdPartys(QString inlog)
{
    auto orders=readorders(inlog);
    double thirdlen=0;

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
                    auto nt=convertMsg2NT(listwithheader,username,from);
                    auto seg=NeuronTree__2__V_NeuronSWC_list(nt).seg[0];
                    thirdlen+=getsegmentlength(seg);
                }
            }
        }
    }
    return thirdlen;
}

void getThirdValues(QStringList files,QString file)
{
    QList<double> values;
    for(auto filr:files)
    {
        values.push_back(doThirdPartys(filr));
    }
    double average=std::accumulate(values.begin(),values.end(),0.0  )*1.0/values.size();
    double var=0;
    for(int j=0;j<values.size();j++)
        var+=pow(values[j]-average,2);
    var/=values.size();
    QFile f(file);
    if(f.open(QIODevice::WriteOnly)){
        QString avestr=QString("average:%1\n").arg(average);
        QString varstr=QString("var=%1\n").arg(var);
        f.write(avestr.toStdString().c_str(),avestr.size());
        f.write(varstr.toStdString().c_str(),varstr.size());
        for(int i=0;i<files.size();i++){
            QString data=QString("%1:%2\n").arg(files[i]).arg(values[i]);
            f.write(data.toStdString().c_str(),data.size());
        }
        f.close();
    }
}

QMap<int,double> getusertimes(QString file)
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
}





#endif // ANALYSELOG_H
