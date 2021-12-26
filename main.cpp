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
    //------------------------------------------------------------------

    //analyse swc
    doaddusertypr(rawswcname,anlyseDir+inbasename+"_addusertype.eswc");

    docheckusertype(rawswcname,anlyseDir+inbasename+"_checkusertype.eswc");

    domodiltytype(rawswcname,anlyseDir+inbasename+"_modiltytype.eswc");

    donodeheatmap(rawswcname,anlyseDir+inbasename+"_heatmap.eswc");

    mergeNts({rawswcname,inDir+inbasename+"_res.eswc"},anlyseDir+inbasename+"_merged.eswc");

    doproof({rawswcname},{inDir+inbasename+"_res.eswc"},
                {anlyseDir+inbasename+"_diff.eswc"},anlyseDir+inbasename+"_proof.txt");
//    analyse log
    getUnUse(rawintructionlist,anlyseDir+inbasename+"_unuse.eswc");

    getspeed(rawintructionlist,rawswcname,anlyseDir+inbasename+"_speed.txt");

//    for(distthres=1;distthres<7;distthres++){
//        for(lengththres=0;lengththres<7;lengththres++){
//            compareA2Bv2(rawswcname,inDir+inbasename+"_res.eswc",QString("%1_%2.eswc").arg(distthres).arg(lengththres));
//        }
//    }

    return 0;
}

