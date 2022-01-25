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
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile f("/Users/huanglei/Desktop/allsoma.csv");
    QMap<QString,QList<CellAPO>> hashmap;
    if(f.open(QIODevice::ReadOnly)){
        QStringList somalist=QString(f.readAll()).split('\n',Qt::SkipEmptyParts);
        for(auto &row:somalist){
            auto somainfo=row.split(',',Qt::SkipEmptyParts);
            CellAPO m;
            m.x=somainfo[1].toDouble();
            m.y=somainfo[2].toDouble();
            m.z=somainfo[3].toDouble();
            m.color.r=0;m.color.b=0;
            hashmap[somainfo[0].trimmed()].push_back(m);
        }
    }
    for(auto it=hashmap.begin();it!=hashmap.end();++it){
        writeAPO_file("/Users/huanglei/Desktop/"+it.key()+".apo",it.value());
    }
    return 0;
}
