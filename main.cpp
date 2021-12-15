#include <QCoreApplication>
#include <stdlib.h>
#include <QMutex>
#include <QDateTime>
#include <QFile>
#include <cstdlib>
#include <cstdio>
#include "analyseswc.h"
#include "analyselog.h"
QString inDir="/Users/huanglei/Desktop/20211207/";
QString inbasename="20211203_testbig";
QString anlyseDir=inDir+"analyse/";
QString rawswcname=inDir+inbasename+".eswc";
QString rawintructionlist=inDir+inbasename+".txt";
QString swcsuffix=".eswc";
QString txtsuffix=".txt";

int main(int argc, char *argv[])
{
//    qInstallMessageHandler(myMessageOutput);
    QCoreApplication a(argc, argv);
//    auto nt=readSWC_file("/Users/huanglei/Desktop/20211207/20211203_testbig.ano.eswc");
//    for(auto &node:nt.listNeuron){
//        if(node.creatmode==0)
//            qDebug()<<node.n;
//    }
//   auto seg=compare("/Users/huanglei/Desktop/20211207/20211203_testbig.ano.eswc",
//                "/Users/huanglei/Desktop/20211207/20211203_order.eswc");
//    auto msgs=readorders("/Users/huanglei/Desktop/20211207/20211203_testbig.txt");
//    for(int i=0;i<msgs.size();i++)
//    {
//        auto locallist=msgs.first(i+1);
//        auto nt=doorders(locallist);
//        auto it=findseg(nt.seg.begin(),nt.seg.end(),seg);
//        if(it!=nt.seg.end()&&it->row.at(0).type==2)
//            qDebug()<<i;
//    }
//verifylog("/Users/huanglei/OneDrive/StudyDB/neu/20211207/20211203_testbig.txt");//寻找log中直接加3号色的区域

    //analyse swc
    doaddusertypr(rawswcname,anlyseDir+inbasename+"_addusertype.eswc");

    docheckusertype(rawswcname,anlyseDir+inbasename+"_checkusertype.eswc");

    domodiltytype(rawswcname,anlyseDir+inbasename+"_modiltytype.eswc");

    donodeheatmap(rawswcname,anlyseDir+inbasename+"_heatmap.eswc");

    doselfconform(rawswcname,anlyseDir+inbasename+"_selfconform.eswc");

    doproof({rawswcname},{inDir+inbasename+"_proof.eswc"},
                {anlyseDir+inbasename+"_proof_analyse.eswc"},anlyseDir+"proof.txt");

    //analyse log
    getUnUse(rawintructionlist,anlyseDir+inbasename+"_unuse.eswc");

    getspeed(rawintructionlist,rawswcname,anlyseDir+inbasename+"_speed.txt");



    //for verify not use
//    getadduserlength("/Users/huanglei/Desktop/20211207/20211203_testbig_afterproof.ano.eswc",
//                     "/Users/huanglei/Desktop/20211207/analyse/20211203_testbig_afterprooflength.txt");

//    return a.exec();
    return 0;
}

