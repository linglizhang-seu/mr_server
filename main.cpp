#include <QCoreApplication>
#include <stdlib.h>
#include <QMutex>
#include <QDateTime>
#include <QFile>
#include <cstdlib>
#include <cstdio>
#include "analyseswc.h"
#include "analyselog.h"
QString inDir="/Users/huanglei/Desktop/18455_00152/";
QString inbasename="18455_00152";
QString anlyseDir=inDir+"analyse/";
QString rawswcname=inDir+inbasename+".eswc";
QString rawintructionlist=inDir+inbasename+".txt";
QString swcsuffix=".eswc";
QString txtsuffix=".txt";

int distthres=3;
int lengththres=1;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    auto nt=readSWC_file("/Users/huanglei/Desktop/18455_00152_unus.eswc");
    auto segs=NeuronTree__2__V_NeuronSWC_list(nt);
    nt=V_NeuronSWC_list__2__NeuronTree(segs);
    writeESWC_file("/Users/huanglei/Desktop/18455_00152_unuse.eswc",nt);
    //------------------------------------------------------------------
//    auto nt=readSWC_file("/Users/huanglei/Desktop/18455_00152_unus.eswc");
//    auto segs=NeuronTree__2__V_NeuronSWC_list(nt);
//    QFile f("/Users/huanglei/Desktop/"+QString::number(getsegmentslength(segs))+"_"+QString::number(getsegmentslength(segs,2)));
//    f.open(QIODevice::WriteOnly);
//    f.close();


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

//    for(distthres=1;distthres<7;distthres++){
//        for(lengththres=0;lengththres<7;lengththres++){
//            compareA2Bv2(rawswcname,inDir+inbasename+"_res.eswc",QString("%1_%2.eswc").arg(distthres).arg(lengththres));
//        }
//    }

//    compareA2Bv2("/Users/huanglei/Desktop/18454_01130_L1A.eswc","/Users/huanglei/Desktop/18454_01130_L1D.eswc","/Users/huanglei/Desktop/18454_01130_A2D.eswc");
//    compareA2Bv2("/Users/huanglei/Desktop/18454_01130_L1D.eswc","/Users/huanglei/Desktop/18454_01130_L2D.eswc","/Users/huanglei/Desktop/18454_01130_1D22D.eswc");
    return 0;
}

