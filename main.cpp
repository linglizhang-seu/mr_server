#include <QCoreApplication>
#include <stdlib.h>
#include <QMutex>
#include <QDateTime>
#include <QFile>
#include <cstdlib>
#include <cstdio>
#include "analyseswc.h"
#include "analyselog.h"
#include <QApplication>
#include <QTableWidget>
#include <QHeaderView>
#include <QTime>
#include <QtGlobal>
QString inDir="/Users/huanglei/Desktop/18455_00152/";
QString inbasename="18455_00152";
QString anlyseDir=inDir+"analyse/";
QString rawswcname=inDir+inbasename+".eswc";
QString rawintructionlist=inDir+inbasename+".txt";
QString swcsuffix=".eswc";
QString txtsuffix=".txt";

int distthres=3;
int lengththres=1;
void length(QString swc)
{
    auto nt=readSWC_file(swc);
    auto segs=NeuronTree__2__V_NeuronSWC_list(nt);
    qDebug()<<swc;
    for(int i=-1;i<10;i++)
    {
        qDebug()<<i<<" "<<getsegmentslength(segs,i);
    }
}
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    {
//        QString logfile= "/Users/huanglei/Desktop/18455_00152.txt";
//        QTableWidget *widget=new QTableWidget(10000,4);
//        QFont font =  widget->horizontalHeader()->font();
//        font.setBold(true);
//        widget->horizontalHeader()->setFont(QFont("Song", 12));
//        widget->horizontalHeader()->setFont(font);
//        widget->setEditTriggers(QAbstractItemView::NoEditTriggers);
//        widget->setShowGrid(true);
//        widget->setWindowTitle("History");
//        QStringList header;
//        header<<"TimeStamp"<<"UserId"<<"Terminal"<<"Operation";
//        widget->setHorizontalHeaderLabels(header);


//        QFile *file=new QFile(logfile);
//        if(!file->open(QIODevice::ReadOnly)) {
//            delete widget;
//            delete file;
//            return 0;
//        }

//        int i=0;
//        QString timestamp,user,client,operation;
//        QString data;
//        while(!file->atEnd()){
//            data=file->readLine(100000000);
//            timestamp=data.left(19);
//            data=data.right(data.size()-27);
//            operation=data.left(data.indexOf(':'));
//            data=data.right(data.size()-data.indexOf(':')-1);
//            auto header=data.split(';',Qt::SkipEmptyParts).at(0).split(' ',Qt::SkipEmptyParts);
//            if(header.size()<=2){
//                ;
//            }
//            user=header[0].trimmed();
//            client=header[1].trimmed();
//            QRandomGenerator64 rd;
////            srand(QDateTime::fromString(timestamp,"yyyy/MM/dd hh:mm:ss").toMSecsSinceEpoch()+time(0));
//            client=QRandomGenerator::global()->generateDouble()>0.8?"Virtual Reality":"Desktop";
//            if(operation.startsWith("drawline")){
//                operation="draw_seg";
//            }else if(operation.startsWith("delline")){
//                operation="del_seg";
//            }else if(operation.startsWith("addmarker")){
//                operation="draw_marker";
//            }else if(operation.startsWith("delmarker")){
//                operation="del_marker";
//            }else if(operation.startsWith("retypeline")){
//                operation="confirm_seg";
//            }
//            widget->setItem(i,0,new QTableWidgetItem(timestamp));
//            widget->setItem(i,1,new QTableWidgetItem(user));
//            widget->setItem(i,2,new QTableWidgetItem(client));
//            widget->setItem(i,3,new QTableWidgetItem(operation));
//            ++i;
//        }
//        widget->show();
//    return a.exec();
//    }

//    //协作重建数据分析
//    //analyse swc
//    doaddusertypr(rawswcname,anlyseDir+inbasename+"_addusertype.eswc");

//    docheckusertype(rawswcname,anlyseDir+inbasename+"_checkusertype.eswc");

//    domodiltytype(rawswcname,anlyseDir+inbasename+"_modiltytype.eswc");

//    donodeheatmap(rawswcname,anlyseDir+inbasename+"_heatmap.eswc");

//    mergeNts({rawswcname,inDir+inbasename+"_res.eswc"},anlyseDir+inbasename+"_merged.eswc");

//    doproof({rawswcname},{inDir+inbasename+"_res.eswc"},
//                {anlyseDir+inbasename+"_diff.eswc"},anlyseDir+inbasename+"_proof.txt");
////    analyse log
//    getUnUse(rawintructionlist,anlyseDir+inbasename+"_unuse.eswc");

//    getspeed(rawintructionlist,rawswcname,anlyseDir+inbasename+"_speed.txt");
//    length(anlyseDir+inbasename+"_unuse.eswc");
//    length(rawswcname);
    //compare 参数调优
//    for(distthres=1;distthres<7;distthres++){
//        for(lengththres=0;lengththres<7;lengththres++){
//            compareA2Bv2(rawswcname,inDir+inbasename+"_res.eswc",QString("%1_%2.eswc").arg(distthres).arg(lengththres));
//        }
//    }

//    compareA2Bv2("/Users/huanglei/Desktop/18454_01130_L1A.eswc","/Users/huanglei/Desktop/18454_01130_L1D.eswc","/Users/huanglei/Desktop/18454_01130_A2D.eswc");
//    compareA2Bv2("/Users/huanglei/Desktop/18454_01130_L1D.eswc","/Users/huanglei/Desktop/18454_01130_L2D.eswc","/Users/huanglei/Desktop/18454_01130_1D22D.eswc");

    //自动化校验
//    {
//        auto nt=readSWC_file("/Users/huanglei/Desktop/whole_image.eswc");
//        auto sges=NeuronTree__2__V_NeuronSWC_list(nt);
//        qDebug()<<"whole_image.eswc "<<getsegmentslength(sges,-1);
//    }

//    {
//        auto nt=readSWC_file("/Users/huanglei/Desktop/18454_00049.swc");
//        auto sges=NeuronTree__2__V_NeuronSWC_list(nt);
//        qDebug()<<"18454_00049.eswc "<<getsegmentslength(sges,-1);
//    }

//    {
//        compareA2Bv2("/Users/huanglei/Desktop/whole_image.eswc","/Users/huanglei/Desktop/18454_00049.swc","/Users/huanglei/Desktop/out.swc");
////        auto nt=readSWC_file("/Users/huanglei/Desktop/out.swc");
////        auto segs=NeuronTree__2__V_NeuronSWC_list(nt);
////        auto ltrue=getsegmentslength(segs,3);
////        nt=readSWC_file("/Users/huanglei/Desktop/18454_00049.swc");
////        segs=NeuronTree__2__V_NeuronSWC_list(nt);
////        auto leffect=getsegmentslength(segs,-1);
////        qDebug()<<ltrue;
////        qDebug()<<leffect;
////        QFile f2("/Users/huanglei/Desktop/"+QString::number(ltrue)+"_"+QString::number(leffect));
////        f2.open(QIODevice::WriteOnly);
////        f2.close();
//    }

//    mergeNts({"/Users/huanglei/Desktop/whole_image.eswc","/Users/huanglei/Desktop/18454_00049.swc"},"/Users/huanglei/Desktop/whole_image_merge.eswc");

    //获取各数据长度
//    length("/Users/huanglei/OneDrive/StudyDB/neu/data/18455_00152/analyse/18455_00152_addusertype.eswc");
//    length("/Users/huanglei/OneDrive/StudyDB/neu/data/18455_00152/analyse/18455_00152_checkusertype.eswc");

//    length("/Users/huanglei/OneDrive/StudyDB/neu/data/18455_00152/analyse/18455_00152_unuse.eswc");
//    length("/Users/huanglei/OneDrive/StudyDB/neu/data/18455_00152/analyse/18455_00152_heatmap.eswc");
//    length("/Users/huanglei/OneDrive/StudyDB/neu/data/18455_00152/analyse/18455_00152_diff.eswc");
//    length("/Users/huanglei/OneDrive/StudyDB/neu/data/18455_00152/analyse/18455_00152_modiltytype.eswc");

//    {
//        //auto grid
//        QString s18454_00049_1="C:/Users/SEU/Desktop/auto/v3d_qt6.exe Z:/TeraconvertedBrain/mouse18454_teraconvert/RES(26298x35000x11041) C:/Users/SEU/Desktop/3.603c C:/Users/SEU/Desktop/huanglei/18454_00049_";

//        QDir dir("./");
//        QFile res("/Users/huanglei/Desktop/18454_00049_res.swc");
//        QStringList msgs;
//        res.open(QIODevice::ReadOnly);

//        for(int blocksize=128;blocksize<=400;blocksize+=128){
//            for(int c=1;c<=10;c++){
//                for(int id_thres=100;id_thres<=500;id_thres+=100){
//                    for(int seg_id=100;seg_id<=500;seg_id+=100){
//                        for(int bord=20;bord<=80;bord+=20){
//                            QString d=QString("%1_%2_%3_%4_%5").arg(blocksize).arg(c).arg(id_thres).arg(seg_id).arg(bord);
//                            QString s18454_00049_2=QString("C:/Users/SEU/Desktop/huanglei/18454_00049_%1/18454_00049_res.swc 14530 10693 3124").arg(d);
//                            msgs.append(QString("%1%2 %3 %4 %5 %6 %7 %8 %9").arg(s18454_00049_1).arg(d).arg(s18454_00049_2).arg(blocksize).arg(c).arg(id_thres).arg(seg_id).arg(bord)
//                                        .arg(QString("whole_%1.eswc").arg(d)).toStdString().c_str());
//                            dir.mkdir("18454_00049_"+d);
//                            res.copy("./18454_00049_"+d+"/18454_00049_res.swc");
//                        }
//                    }
//                }
//            }
//        }

//        int k=msgs.size()/20;
//        for(int i=0;i<20;i++)
//        {
//            QFile f(QString("task_%1").arg(i));
//            f.open(QIODevice::WriteOnly);
//            f.write(msgs.sliced(i*k,k).join('\n').toStdString().c_str());
//            f.close();
//        }
//    }
//    dologfile("/Users/huanglei/Desktop/18455_00152_1000.txt","/Users/huanglei/Desktop/18455_00152_1000.eswc");
//    dologfile("/Users/huanglei/Desktop/18455_00152_2000.txt","/Users/huanglei/Desktop/18455_00152_2000.eswc");
//    dologfile("/Users/huanglei/Desktop/18455_00152_3000.txt","/Users/huanglei/Desktop/18455_00152_3000.eswc");
//    dologfile("/Users/huanglei/Desktop/18455_00152_4000.txt","/Users/huanglei/Desktop/18455_00152_4000.eswc");
//    dologfile("/Users/huanglei/Desktop/18455_00152_5000.txt","/Users/huanglei/Desktop/18455_00152_5000.eswc");
//    dologfile("/Users/huanglei/Desktop/18455_00152_6000.txt","/Users/huanglei/Desktop/18455_00152_6000.eswc");
//    dologfile("/Users/huanglei/Desktop/18455_00152.txt","/Users/huanglei/Desktop/18455_00152_7000.eswc");

    compareA2Bv2("/Users/huanglei/Desktop/18454_01130_DAH_stamp_2019_11_19_17_49.ano.eswc"
                 ,"/Users/huanglei/Desktop/18454_01130_DAH_YLL_SYY_stamp_2019_12_10_09_09.ano.eswc"
                 ,"/Users/huanglei/Desktop/10002000.eswc");
    compareA2Bv2("/Users/huanglei/Desktop/18454_01130_DAH_YLL_stamp_2019_11_26_14_22.ano.eswc"
                 ,"/Users/huanglei/Desktop/18454_01130_DAH_YLL_stamp_2019_12_09_13_47.ano.eswc"
                 ,"/Users/huanglei/Desktop/20007000.eswc");


    return 0;
}
