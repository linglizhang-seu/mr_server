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
//        auto nt=readSWC_file("/Users/huanglei/Desktop/18867_6003_x18592_y6204.swc");
//        auto sges=NeuronTree__2__V_NeuronSWC_list(nt);
//        qDebug()<<"18454_00049.eswc "<<getsegmentslength(sges,-1);
//    }

//    {
//        compareA2Bv2("/Users/huanglei/Desktop/whole_image.eswc","/Users/huanglei/Desktop/18867_6003_x18592_y6204.swc","/Users/huanglei/Desktop/out.swc");
//        auto nt=readSWC_file("/Users/huanglei/Desktop/out.swc");
//        auto segs=NeuronTree__2__V_NeuronSWC_list(nt);
//        auto ltrue=getsegmentslength(segs,3);
//        nt=readSWC_file("/Users/huanglei/Desktop/18454_00049.swc");
//        segs=NeuronTree__2__V_NeuronSWC_list(nt);
//        auto leffect=getsegmentslength(segs,-1);
//        qDebug()<<ltrue;
//        qDebug()<<leffect;
//        QFile f2("/Users/huanglei/Desktop/"+QString::number(ltrue)+"_"+QString::number(leffect));
//        f2.open(QIODevice::WriteOnly);
//        f2.close();
//    }

//    mergeNts({"/Users/huanglei/Desktop/whole_image.eswc","/Users/huanglei/Desktop/18454_00049.swc"},"/Users/huanglei/Desktop/whole_image_merge.eswc");

    //获取各数据长度
//    length("/Users/huanglei/OneDrive/StudyDB/neu/data/18455_00152/analyse/18455_00152_addusertype.eswc");
//    length("/Users/huanglei/OneDrive/StudyDB/neu/data/18455_00152/analyse/18455_00152_checkusertype.eswc");

//    length("/Users/huanglei/OneDrive/StudyDB/neu/data/18455_00152/analyse/18455_00152_unuse.eswc");
//    length("/Users/huanglei/OneDrive/StudyDB/neu/data/18455_00152/analyse/18455_00152_heatmap.eswc");
//    length("/Users/huanglei/OneDrive/StudyDB/neu/data/18455_00152/analyse/18455_00152_diff.eswc");
//    length("/Users/huanglei/OneDrive/StudyDB/neu/data/18455_00152/analyse/18455_00152_modiltytype.eswc");

    {
        //auto grid
        QMap<QString,QString> imageMap;
        imageMap["18462"] ="Z:/TeraconvertedBrain/mouse18462_teraconvert/RES(27748x32200x10578)";
        imageMap["17781"] ="Z:/TeraconvertedBrain/mouse17781_teraconvert/RES(27300x17361x5510)";
        imageMap["191812"]="Z:/TeraconvertedBrain/mouse191812_teraconvert/RES(30801x18911x11657)";
        imageMap["17543"] ="Z:/TeraconvertedBrain/mouse17543_teraconvert/RES(25201x18911x10217)";
        imageMap["18458"] ="Z:/TeraconvertedBrain/mouse18458_teraconvert/RES(25376x40600x10333)";
        imageMap["18869"] ="Z:/TeraconvertedBrain/mouse18869_teraconvert/RES(16915x29400x10923)";
        imageMap["18465"] ="Z:/TeraconvertedBrain/mouse18465_teraconvert/RES(26000x36000x10301)";
        imageMap["236174"]="Z:/TeraconvertedBrain/mouse236174_teraconvert/RES(36400x23814x12061)";
        imageMap["18452"] ="Z:/TeraconvertedBrain/mouse18452_teraconvert/RES(23423x36400x11028)";
        imageMap["191803"]="Z:/TeraconvertedBrain/mouse191803_teraconvert_12bit/RES(29401x17848x11712)";
        imageMap["18864"] ="Z:/TeraconvertedBrain/mouse18864_teraconvert/RES(35001x27299x10392)";
        imageMap["18453"] ="Z:/TeraconvertedBrain/mouse18453_teraconvert/RES(24400x39200x10538)";
        imageMap["191801"]="Z:/TeraconvertedBrain/mouse191801_teraconvert_12bit/RES(28001x17865x11151)";
        imageMap["18463"] ="Z:/TeraconvertedBrain/mouse18463_teraconvert/RES(17478x32200x10842)";
        imageMap["18867"] ="Z:/TeraconvertedBrain/mouse18867_teraconvert/RES(25376x40600x11203)";
        imageMap["182725"]="Z:/TeraconvertedBrain/mouse182725_teraconvert/RES(30801x20821x11464)";
        imageMap["17302"] ="Z:/TeraconvertedBrain/mouse17302_teraconvert/RES(54600x34412x9847)";
        imageMap["18454"] ="Z:/TeraconvertedBrain/mouse18454_teraconvert/RES(26298x35000x11041)";
        imageMap["18457"] ="Z:/TeraconvertedBrain/mouse18457_teraconvert/RES(24275x37800x10955)";
        imageMap["17545"] ="Z:/TeraconvertedBrain/mouse17545_teraconvert/RES(27300x17994x5375)";
        imageMap["18455"] ="Z:/TeraconvertedBrain/mouse18455_teraconvert/RES(23376x40600x10892)";
        imageMap["17300"] ="Z:/TeraconvertedBrain/mouse17300_teraconvert/RES(54600x37230x9954)";
        imageMap["18868"] ="Z:/TeraconvertedBrain/mouse18868_teraconvert/RES(24001x36401x10995)";
        imageMap["17109"] ="Z:/TeraconvertedBrain/mouse17109_teraconvert/RES(35000x22793x10553)";
        imageMap["17787"] ="Z:/TeraconvertedBrain/mouse17787_teraconvert/RES(24275x36400x11478)";
        imageMap["17788"] ="Z:/TeraconvertedBrain/mouse17788_teraconvert/RES(27188x35000x11120)";
        imageMap["18464"] ="Z:/TeraconvertedBrain/mouse18464_teraconvert/RES(26352x35000x10431)";
        imageMap["18470"] ="Z:/TeraconvertedBrain/mouse18470_teraconvert/RES(27300x15977x5113)";
        imageMap["17782"] ="Z:/TeraconvertedBrain/mouse17782_teraconvert/RES(28000x45000x11786)";

        QDir autodir("/Users/huanglei/Desktop");
        autodir.mkdir("autoHL20220214");
        if(!autodir.cd("autoHL20220214")){
            qDebug()<<"cd autoHL20220214 error";
        }else{
            qDebug()<<autodir;
        }

        QDir dir("/Users/huanglei/Downloads/1708_unregistered");
        auto files=dir.entryInfoList(QDir::Files,QDir::Name);
        QMap<QString,QStringList> swcMap;
        for(auto &file:files){
            swcMap[file.baseName().split('_')[0]].push_back(file.fileName());
        }

        for(auto &key:swcMap.keys()){
            QFile file(key+".txt");
            if(file.open(QIODevice::WriteOnly)){
                for(auto &value:swcMap[key]){
                    QDir tmpdir=autodir;
                    if(!tmpdir.mkdir(value.split('.').at(0)+"skip")){
                        qDebug()<<tmpdir;
                    }

                    if(!QFile::copy("/Users/huanglei/Downloads/1708_unregistered/"+value,"/Users/huanglei/Desktop/autoHL20220214/"+value.split('.').at(0)+"skip/"+value)){
                        qDebug()<<"error "<<value;
                    }
                    QString data=QString("C:/Users/SEU/Desktop/auto/v3d_qt6.exe %1 C:/Users/SEU/Desktop/3.603c D:/%2 D:/%2/%3\n")
                            .arg(imageMap[key])
                            .arg(QString("autoHL20220214/%1skip").arg(value.split('.').at(0)))
                            .arg(value);
                    file.write(data.toStdString().c_str());
                }
            }else{
                qDebug()<<"file open failed";
            }
            file.close();
        }
    }


//    dologfile("/Users/huanglei/Desktop/18455_00152_1000.txt","/Users/huanglei/Desktop/18455_00152_1000.eswc");
//    dologfile("/Users/huanglei/Desktop/18455_00152_2000.txt","/Users/huanglei/Desktop/18455_00152_2000.eswc");
//    dologfile("/Users/huanglei/Desktop/18455_00152_3000.txt","/Users/huanglei/Desktop/18455_00152_3000.eswc");
//    dologfile("/Users/huanglei/Desktop/18455_00152_4000.txt","/Users/huanglei/Desktop/18455_00152_4000.eswc");
//    dologfile("/Users/huanglei/Desktop/18455_00152_5000.txt","/Users/huanglei/Desktop/18455_00152_5000.eswc");
//    dologfile("/Users/huanglei/Desktop/18455_00152_6000.txt","/Users/huanglei/Desktop/18455_00152_6000.eswc");
//    dologfile("/Users/huanglei/Desktop/18455_00152.txt","/Users/huanglei/Desktop/18455_00152_7000.eswc");

//    QDir target("/Users/huanglei/Desktop/target");
//    auto entrylist=target.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot,QDir::Name);

//    for(auto &entry:entrylist){
//        auto tmpDir=QDir(entry.absoluteFilePath());
//        auto tmpentrylist=tmpDir.entryInfoList(QDir::Files,QDir::Name);
//        QString respath="";
//        QString autopath="whole_image.eswc";
//        QString resname="";
//        for(auto &tmpentry:tmpentrylist){
//            if(!tmpentry.baseName().contains("whole_image")){
//                respath=tmpentry.absoluteFilePath();
//                resname=tmpentry.baseName();
//            }else{
//                autopath=tmpentry.absoluteFilePath();
//            }
//        }
//        compareA2Bv2(autopath,respath,"/Users/huanglei/Desktop/target/"+resname+"_compare.eswc");

//    }


    return 0;
}
