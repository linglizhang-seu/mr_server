#include <QCoreApplication>
#include <stdlib.h>
#include <QMutex>
#include <QDateTime>
#include <QFile>
#include <cstdlib>
#include <cstdio>
#include "analyseswc.h"
#include "analyselog.h"
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


    //analyse log
    getUnUse("/Users/huanglei/Desktop/20211207/20211203_testbig.txt",
             "/Users/huanglei/Desktop/20211207/20211203_testbig_unuse.eswc");


    //analyse swc
    doaddusertypr("/Users/huanglei/Desktop/20211207/20211203_testbig.ano.eswc",
                  "/Users/huanglei/Desktop/20211207/20211203_testbig_addusertype.eswc");

    docheckusertype("/Users/huanglei/Desktop/20211207/20211203_testbig.ano.eswc",
                  "/Users/huanglei/Desktop/20211207/20211203_testbig_checkusertype.eswc");

    domodiltytype("/Users/huanglei/Desktop/20211207/20211203_testbig.ano.eswc",
                  "/Users/huanglei/Desktop/20211207/20211203_testbig_modiltytype.eswc");

    getadduserlength("/Users/huanglei/Desktop/20211207/20211203_testbig.ano.eswc",
                     "/Users/huanglei/Desktop/20211207/20211203_testbig_adduserlength.txt");

    getretypeuserlength("/Users/huanglei/Desktop/20211207/20211203_testbig.ano.eswc",
                     "/Users/huanglei/Desktop/20211207/20211203_testbig_retypeuserlength.txt");

    doheatmap("/Users/huanglei/Desktop/20211207/20211203_testbig.ano.eswc",
                     "/Users/huanglei/Desktop/20211207/20211203_testbig_heatmap.swc");
//    return a.exec();
    return 0;
}

