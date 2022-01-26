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
    QMap<QString,QVector<int>> mmap;
    mmap["191799"]={1492,1925,689};
    mmap["191798"]={1180,1925,669};
    mmap["191797"]={1120,1925,699};
    QMap<QString,QList<CellAPO>> hashmap;
    if(f.open(QIODevice::ReadOnly)){
        QStringList somalist=QString(f.readAll()).split('\n',Qt::SkipEmptyParts);
        for(auto &row:somalist){
            auto somainfo=row.split(',',Qt::SkipEmptyParts);
            CellAPO m;
            m.x=somainfo[1].toDouble()/16;
            m.y=somainfo[2].toDouble()/16;
            m.z=somainfo[3].toDouble()/16;
            m.color.r=0;m.color.b=0;
            if(mmap.contains(somainfo[0].trimmed())){
                if(m.x>=mmap[somainfo[0].trimmed()].at(0)||m.y>=mmap[somainfo[0].trimmed()].at(1)||m.z>=mmap[somainfo[0].trimmed()].at(2)){
                    qDebug()<<row;
                    continue;
                }
            }
            hashmap[somainfo[0].trimmed()].push_back(m);
        }
    }
    for(auto it=hashmap.begin();it!=hashmap.end();++it){
        writeAPO_file("/Users/huanglei/Desktop/"+it.key()+".apo",it.value());
    }
    return 0;
}
