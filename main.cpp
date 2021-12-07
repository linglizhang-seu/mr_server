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

    //analyse log
    getUnUse("/Users/huanglei/Desktop/20211207/20211203_testbig.txt",
             "/Users/huanglei/Desktop/20211207/20211203_order.eswc");
    compare("/Users/huanglei/Desktop/20211207/20211203_testbig.ano.eswc",
            "/Users/huanglei/Desktop/20211207/20211203_order.eswc");

    //analyse swc
//    doaddusertypr("/Users/huanglei/Desktop/20211207/20211203_testbig.ano.eswc",
//                  "/Users/huanglei/Desktop/20211207/20211203_testbig_addusertype.eswc");

//    docheckusertype("/Users/huanglei/Desktop/20211207/20211203_testbig.ano.eswc",
//                  "/Users/huanglei/Desktop/20211207/20211203_testbig_checkusertype.eswc");

//    domodiltytype("/Users/huanglei/Desktop/20211207/20211203_testbig.ano.eswc",
//                  "/Users/huanglei/Desktop/20211207/20211203_testbig_modiltytype.eswc");

//    getadduserlength("/Users/huanglei/Desktop/20211207/20211203_testbig.ano.eswc",
//                     "/Users/huanglei/Desktop/20211207/20211203_testbig_adduserlength.txt");

//    getretypeuserlength("/Users/huanglei/Desktop/20211207/20211203_testbig.ano.eswc",
//                     "/Users/huanglei/Desktop/20211207/20211203_testbig_retypeuserlength.txt");

//    doheatmap("/Users/huanglei/Desktop/20211207/20211203_testbig.ano.eswc",
//                     "/Users/huanglei/Desktop/20211207/20211203_testbig_heatmap.swc");
//    return a.exec();
    return 0;
}

