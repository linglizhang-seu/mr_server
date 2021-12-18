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
#include "swcutils.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

//    mergeNts({"/Users/huanglei/Desktop/test/18455_00152.eswc",
//              "/Users/huanglei/Desktop/test/18455_00152_res.eswc"
//             },"/Users/huanglei/Desktop/test/18455_00152_merged.eswc");

    compareA2Bv2("/Users/huanglei/Desktop/test/18455_00152.eswc","/Users/huanglei/Desktop/test/18455_00152_res.eswc");



    //------------------------------------------------------------------

//    //analyse swc
//    doaddusertypr(rawswcname,anlyseDir+inbasename+"_addusertype.eswc");

//    docheckusertype(rawswcname,anlyseDir+inbasename+"_checkusertype.eswc");

//    domodiltytype(rawswcname,anlyseDir+inbasename+"_modiltytype.eswc");

//    donodeheatmap(rawswcname,anlyseDir+inbasename+"_heatmap.eswc");

//    doselfconform(rawswcname,anlyseDir+inbasename+"_selfconform.eswc");

////    doproof({rawswcname},{inDir+inbasename+"_proof.eswc"},
////                {anlyseDir+inbasename+"_proof_analyse.eswc"},anlyseDir+"proof.txt");

//    //analyse log
//    getUnUse(rawintructionlist,anlyseDir+inbasename+"_unuse.eswc");

//    getspeed(rawintructionlist,rawswcname,anlyseDir+inbasename+"_speed.txt");



    //for verify not use
//    getadduserlength("/Users/huanglei/Desktop/20211207/20211203_testbig_afterproof.ano.eswc",
//                     "/Users/huanglei/Desktop/20211207/analyse/20211203_testbig_afterprooflength.txt");

//    return a.exec();

            return 0;
}

