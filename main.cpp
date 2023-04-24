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
#include "basic_c_fun/basic_surf_objs.h"

QString inDir3="H:/Neu/Final_all_20_stype_second/8_acc_log";
QString inbasename="18455_00152";
//QString anlyseDir=inDir+"analyse/";
//QString rawswcname=inDir+inbasename+".eswc";
//QString rawintructionlist=inDir+inbasename+".txt";
QString swcsuffix=".eswc";
QString txtsuffix=".txt";

QString inDir1="H:/Neu/Final_all_20_stype_second/3_connect_swc/result";
QString inDir2="H:/Neu/Final_all_20_stype_second/release/";
//QString inDir4="H:/Neu/Final_all_20_stype_second/test";
//QString inDir3="H:/Neu/Final_all_20_stype_second/4.2_before_sort_proof_swc_manual/";
//QString outDir="H:/Neu/Final_all_20_stype_second/4_before_sort_proof_swc/";
QString outDir="H:/Neu/Final_all_20_stype_second/8_acc_analyse/1_analyse_swc";
//QString inDir1="H:/Neu/yizhixing/towang";
int distthres=4;
int lengththres=2;
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
//    QDir dir(inDir);
//    QFileInfoList swclist=dir.entryInfoList();
//    int i=0;
//    while(i<swclist.size()){
//        QFileInfo swcfile = swclist.at(i);
//        QString swc=swcfile.filePath();
//        QFile accfile=swcfile.filePath()+"_acc.txt";
//        QString acc=accfile.fileName();
//        auto nt=readSWC_file(swc);
//        auto segs=NeuronTree__2__V_NeuronSWC_list(nt);
//        QFile f(acc);
//        if(!f.open(QIODevice::WriteOnly)) return 0;
//        QString data=QString("%1 %2\n").arg("right").arg("error");
//        f.write(data.toStdString().c_str(),data.size());
//        QString data2=QString("%1 %2\n").arg(getsegmentslength(segs,1)).arg(getsegmentslength(segs,2));
//        f.write(data2.toStdString().c_str(),data2.size());
//        i++;
//    }

//        QDir dir(inDir);
//        QFileInfoList swclist=dir.entryInfoList();
//        int i=0;
//        while(i<swclist.size()){
//            QFileInfo swcfile = swclist.at(i);
//            QString swc=swcfile.filePath();
//            QFile accfile=swcfile.filePath()+"_acc.txt";
//            QString acc=accfile.fileName();
//            auto nt=readSWC_file(swc);
//            auto segs=NeuronTree__2__V_NeuronSWC_list(nt);
//            QFile f(acc);
//            if(!f.open(QIODevice::WriteOnly)) return 0;
//            QString data=QString("%1 %2\n").arg("total").arg("retype");
//            f.write(data.toStdString().c_str(),data.size());
//            QString data2=QString("%1 %2\n").arg(getsegmentslength(segs)).arg(getsegmentslength(segs,2));
//            f.write(data2.toStdString().c_str(),data2.size());

//            i++;

//        }


//    QDir dir(inDir1);
//    dir.setFilter(QDir::Files | QDir::NoSymLinks);
//    QFileInfoList list=dir.entryInfoList();
//    int i=0;
//    while(i<list.size()){
//        QFileInfo rawfile=list.at(i);
//        QString raw=rawfile.filePath();
//        QFile unusefile=outDir+rawfile.baseName()+"_unuse.eswc";
//        QString unuse = unusefile.fileName();
//        getUnUse(raw,unuse);
//        i++;

//    }


//    QFile f("H:/Neu/20stype/release/juzheng.txt");
//    if(!f.open(QIODevice::WriteOnly)) return 0;
//    QString data=QString("%1 %2 %3 %4 %5 %6 %7 %8 %9 %10 %11 %12 %13 %14 %15 %16 %17 %18 %19 %20 %21 %22 %23 %24 %25 %26 %27\n").arg("38").arg("44").arg("75").arg("83").arg("89").arg("93").arg("94").arg("98").arg("104").arg("119").arg("125").arg("135").arg("148").arg("188").arg("238").arg("248").arg("393").arg("398").arg("453").arg("458").arg("459").arg("464").arg("489").arg("496").arg("548").arg("1583").arg("1588");
//    f.write(data.toStdString().c_str(),data.size());
//    QString data2=QString("%1 %2 %3 %4 %5 %6 %7 %8 %9 %10 %11 %12 %13 %14 %15 %16 %17 %18 %19 %20 %21 %22 %23 %24 %25 %26 %27\n").arg(getsegmentslength(segs,38)).arg(getsegmentslength(segs,44)).arg(getsegmentslength(segs,75)).arg(getsegmentslength(segs,83)).arg(getsegmentslength(segs,89)).arg(getsegmentslength(segs,93)).arg(getsegmentslength(segs,94)).arg(getsegmentslength(segs,98)).arg(getsegmentslength(segs,104)).arg(getsegmentslength(segs,119)).arg(getsegmentslength(segs,125)).arg(getsegmentslength(segs,135)).arg(getsegmentslength(segs,148)).arg(getsegmentslength(segs,188)).arg(getsegmentslength(segs,238)).arg(getsegmentslength(segs,248)).arg(getsegmentslength(segs,393)).arg(getsegmentslength(segs,398)).arg(getsegmentslength(segs,453)).arg(getsegmentslength(segs,458)).arg(getsegmentslength(segs,459)).arg(getsegmentslength(segs,464)).arg(getsegmentslength(segs,489)).arg(getsegmentslength(segs,496)).arg(getsegmentslength(segs,548)).arg(getsegmentslength(segs,1583)).arg(getsegmentslength(segs,1588));
//    f.write(data2.toStdString().c_str(),data2.size());

//    qDebug()<<"len416"<<getsegmentslength(segs,148);

//    QDir dir(inDir1);
//    dir.setFilter(QDir::Files | QDir::NoSymLinks);
////    QDir dir2(inDir2);
//    QFileInfoList list=dir.entryInfoList();
////    qDebug()<<"list"<<list;
////    QFileInfoList list2=dir2.entryInfoList();
////    qDebug()<<"list2"<<list2;
//    int i=0;
//    while(i<list.size()){
//        QFileInfo rawfile=list.at(i);
//        QFile difffile=outDir+rawfile.baseName()+"_diff.eswc";
//        QFile prooffile=outDir+rawfile.baseName()+"_proof.txt";
//        QString raw=rawfile.filePath();
//        QString release="H:/Neu/addedMTG/release_705/00705_P007_T01-S001_MTG_R0678_LJ-20220530_LJ.res.eswc";
////        QString release= inDir2+rawfile.baseName()+"_output.eswc";
//        QString diff=difffile.fileName();
//        QString proof=prooffile.fileName();
//        qDebug()<<"raw:"<<raw<<"release:"<<release<<"diff"<<diff<<"proof"<<proof;
//        i++;
//        doproof({raw},{release},{diff},{proof});
////        i++;
//    }

//    QDir dir(inDir1);
//    dir.setFilter(QDir::Files | QDir::NoSymLinks);
//    QFileInfoList list=dir.entryInfoList();
//    int i=0;
//    while(i<list.size())
//    {
//        QFileInfo fileinfo = list.at(i);
//        qDebug()<<"fileinfo"<<fileinfo;
//        QString infile = fileinfo.filePath();
//        qDebug()<<"infile:"<<infile;
//        QString filename = fileinfo.baseName();
//        qDebug()<<"filename"<<filename;
//        QString diff=inDir3+filename+"_diff.swc";
//        QFile proooffile(outDir+filename+"_proof.txt");
//        QString release=inDir2+filename.split("_")[1]+".res.swc";
//        QString prooof=proooffile.fileName();
//        qDebug()<<"raw:"<<infile<<"diff"<<diff<<"proof"<<prooof<<"release"<<release;
//        doproofV2({infile},{release},{diff},{prooof});
//        i++;
//    }


//    QDir dir(inDir1);
//    QFileInfoList list=dir.entryInfoList();
//    int i=0;
//    while(i<list.size()){
//        QFileInfo rawfile=list.at(i);
////        QFileInfo releasefile=list2.at(i);

//        QFile difffile=outDir+rawfile.filePath()+"_diff.eswc";
////        qDebug()<<"difffile"<<difffile.fileName();
//        QFile prooffile=outDir+rawfile.filePath()+"_proof.txt";
////        if(!difffile.open(QIODevice::Append))
////        {
////            return 0;
////        }
////        if(!prooffile.open(QIODevice::Append))
////        {
////            return 0;
////        }


//        QString raw=rawfile.filePath();
////        QString release=releasefile.filePath();
//        QString diff=difffile.fileName();
//        QString proof=prooffile.fileName();
//        QString release= inDir2+rawfile.baseName().split("_")[1]+".res.eswc";

//        qDebug()<<"raw:"<<raw<<"diff"<<diff<<"proof"<<proof;

////        qDebug()<<"difffile"<<difffile.fileName();
////        qDebug()<<"raw"<<raw;

////        do
//        proof({raw},{release},{diff},{proof});
//        i++;
//    }


//    QDir dir(inDir1);
//     dir.setFilter(QDir::Files | QDir::NoSymLinks);
//     QFileInfoList list = dir.entryInfoList();
//     qDebug()<<"list"<<list;
//     int i =0;
//     while(i<list.length())
//     {
//         QFileInfo fileinfo = list.at(i);
//         qDebug()<<"fileinfo"<<fileinfo;
//         QString infile = fileinfo.filePath();
//         qDebug()<<"infile:"<<infile;
//         QString filename = fileinfo.baseName();
//         qDebug()<<"filename"<<filename;
//         QFile difffile(outDir+filename+"_diff.swc");
//         QFile proooffile(outDir+filename+"_proof.txt");
//         QString release=inDir2+filename.split("_")[1]+".res.swc";
//         QString diff=difffile.fileName();
//         QString prooof=proooffile.fileName();
//         qDebug()<<"raw:"<<infile<<"diff"<<diff<<"proof"<<prooof<<"release"<<release;

//         doproof({infile},{release},{diff},{prooof});
//         i++;

//     }




    QDir dir(inDir3);
//    QDir dirout(outDir);
    QFileInfoList list=dir.entryInfoList();
    qDebug()<<"list"<<list;
    int i=0;

    while(i<list.size()){

        QFileInfo fileinfo = list.at(i);
        qDebug()<<"fileinfo"<<fileinfo;
        QString infile = fileinfo.filePath();
        qDebug()<<"infile:"<<infile;
        QString filename = fileinfo.baseName();
        qDebug()<<"filename"<<filename;
        QFile outfile(outDir+"/"+filename+".eswc");
        if(!outfile.open(QIODevice::ReadWrite))
           return 0;

//        QFile unusefile(outDir+"/"+filename+"_unuse.eswc");
//        QFile speedtxt(outDir+"/"+filename+"_speed.txt");
//        QFile addUserTypeFile(outDir+"/"+filename+"_addusertype.eswc");
//        QFile checkUserTypeFile(outDir+"/"+filename+"_checkusertype.eswc");
//        QFile heatmapFile(outDir+"/"+filename+"_heatmap.eswc");

        QString out = outfile.fileName();
//        QString unuse = unusefile.fileName();
//        QString speed = speedtxt.fileName();
//        QString addUserType= addUserTypeFile.fileName();
//        QString checkUserType=checkUserTypeFile.fileName();
//        QString heatmap = heatmapFile.fileName();

        qDebug()<<"out2"<<out;

        dologfile(infile,out);
//        getUnUse(infile,unuse);
//        getspeed(infile,out,speed);
//        doaddusertypr(out,addUserType);
//        docheckusertype(out,checkUserType);
//        donodeheatmap(out,heatmap);
        i++;

////        QFileInfo rawtext=list.at(i);
////        QFile swcfile=outDir+rawtext.baseName()+".eswc";
//////        QFile speedfile=outDir+rawtext.baseName()+"_speed.txt";
////        QString raw=rawtext.filePath();
////        QString swc=swcfile.fileName();
//////        QString speed=speedfile.fileName();
////        qDebug()<<"raw"<<raw;
////        qDebug()<<"swc"<<swc;
//////        qDebug()<<"speed"<<speed;


////        dologfile(raw,
////                  swc);
//////        getspeed(raw,swc,speed);



////        doaddusertypr("C:/Users/Braintell/Desktop/analyseMTG/00490_P005_T01-S006_MFG_R0490_LJ-20220525_LJ_05.eswc",
////                      "C:/Users/Braintell/Desktop/analyseMTG/00490_P005_T01-S006_MFG_R0490_LJ-20220525_LJ_05_addusertype.eswc");
//////        docheckusertype(rawswcname,anlyseDir+inbasename+"_checkusertype.eswc");
////        donodeheatmap("C:/Users/Braintell/Desktop/analyseMTG/00490_P005_T01-S006_MFG_R0490_LJ-20220525_LJ_05.eswc",
////                      "C:/Users/Braintell/Desktop/analyseMTG/00490_P005_T01-S006_MFG_R0490_LJ-20220525_LJ_05_heatmap.eswc");
////        getUnUse("H:/Neu/final20stype/juzheng/test/test2/17302_00024_output.txt",
////                 "H:/Neu/final20stype/juzheng/test/test2/17302_00024_output_unuse.eswc");

////            getspeed("C:/Users/Braintell/Desktop/Neuroverse_final/testunse/log/18458_00574.txt",
////                     "C:/Users/Braintell/Desktop/Neuroverse_final/testunse/oriswc/18458_00574.eswc",
////                     "C:/Users/Braintell/Desktop/Neuroverse_final/testunse/oriswc/18458_00574_speed.txt");

    }
//    doproof({"H:/Neu/Final_all_20_stype_second/18453_7010_x8769_y4509.ano.eswc"},{"H:/Neu/Final_all_20_stype_second/release/7010.res.swc"},{"H:/Neu/Final_all_20_stype_second/7010_diff.eswc"},{"H:/Neu/Final_all_20_stype_second/7010_proof.txt"});




//    doproof({rawswcname},{inDir+inbasename+"_res.eswc"},
//                {anlyseDir+inbasename+"_diff.eswc"},anlyseDir+inbasename+"_proof.txt");
//
//    auto nt=readSWC_file("/Users/huanglei/Desktop/18455_00152_modiltytype.eswc");
//    for(auto &node:nt.listNeuron){
//        node.x*=0.2;
//        node.y*=0.2;
//        node.z*=0.1;
//    }
//    writeESWC_file("/Users/huanglei/Desktop/18455_00152_modiltytype.eswc",nt);
//        length("/Users/huanglei/Desktop/18455_00152_modiltytype.eswc");
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


////    analyse log
//    getUnUse(rawintructionlist,anlyseDir+inbasename+"_unuse.eswc");

//    getspeed(rawintructionlist,rawswcname,anlyseDir+inbasename+"_speed.txt");
//    length(anlyseDir+inbasename+"_unuse.eswc");

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

//    {
//        //auto grid
//        QMap<QString,QString> imageMap;
//        imageMap["18462"] ="Z:/TeraconvertedBrain/mouse18462_teraconvert/RES(27748x32200x10578)";
//        imageMap["17781"] ="Z:/TeraconvertedBrain/mouse17781_teraconvert/RES(27300x17361x5510)";
//        imageMap["191812"]="Z:/TeraconvertedBrain/mouse191812_teraconvert/RES(30801x18911x11657)";
//        imageMap["17543"] ="Z:/TeraconvertedBrain/mouse17543_teraconvert/RES(25201x18911x10217)";
//        imageMap["18458"] ="Z:/TeraconvertedBrain/mouse18458_teraconvert/RES(25376x40600x10333)";
//        imageMap["18869"] ="Z:/TeraconvertedBrain/mouse18869_teraconvert/RES(16915x29400x10923)";
//        imageMap["18465"] ="Z:/TeraconvertedBrain/mouse18465_teraconvert/RES(26000x36000x10301)";
//        imageMap["236174"]="Z:/TeraconvertedBrain/mouse236174_teraconvert/RES(36400x23814x12061)";
//        imageMap["18452"] ="Z:/TeraconvertedBrain/mouse18452_teraconvert/RES(23423x36400x11028)";
//        imageMap["191803"]="Z:/TeraconvertedBrain/mouse191803_teraconvert_12bit/RES(29401x17848x11712)";
//        imageMap["18864"] ="Z:/TeraconvertedBrain/mouse18864_teraconvert/RES(35001x27299x10392)";
//        imageMap["18453"] ="Z:/TeraconvertedBrain/mouse18453_teraconvert/RES(24400x39200x10538)";
//        imageMap["191801"]="Z:/TeraconvertedBrain/mouse191801_teraconvert_12bit/RES(28001x17865x11151)";
//        imageMap["18463"] ="Z:/TeraconvertedBrain/mouse18463_teraconvert/RES(17478x32200x10842)";
//        imageMap["18867"] ="Z:/TeraconvertedBrain/mouse18867_teraconvert/RES(25376x40600x11203)";
//        imageMap["182725"]="Z:/TeraconvertedBrain/mouse182725_teraconvert/RES(30801x20821x11464)";
//        imageMap["17302"] ="Z:/TeraconvertedBrain/mouse17302_teraconvert/RES(54600x34412x9847)";
//        imageMap["18454"] ="Z:/TeraconvertedBrain/mouse18454_teraconvert/RES(26298x35000x11041)";
//        imageMap["18457"] ="Z:/TeraconvertedBrain/mouse18457_teraconvert/RES(24275x37800x10955)";
//        imageMap["17545"] ="Z:/TeraconvertedBrain/mouse17545_teraconvert/RES(27300x17994x5375)";
//        imageMap["18455"] ="Z:/TeraconvertedBrain/mouse18455_teraconvert/RES(23376x40600x10892)";
//        imageMap["17300"] ="Z:/TeraconvertedBrain/mouse17300_teraconvert/RES(54600x37230x9954)";
//        imageMap["18868"] ="Z:/TeraconvertedBrain/mouse18868_teraconvert/RES(24001x36401x10995)";
//        imageMap["17109"] ="Z:/TeraconvertedBrain/mouse17109_teraconvert/RES(35000x22793x10553)";
//        imageMap["17787"] ="Z:/TeraconvertedBrain/mouse17787_teraconvert/RES(24275x36400x11478)";
//        imageMap["17788"] ="Z:/TeraconvertedBrain/mouse17788_teraconvert/RES(27188x35000x11120)";
//        imageMap["18464"] ="Z:/TeraconvertedBrain/mouse18464_teraconvert/RES(26352x35000x10431)";
//        imageMap["18470"] ="Z:/TeraconvertedBrain/mouse18470_teraconvert/RES(27300x15977x5113)";
//        imageMap["17782"] ="Z:/TeraconvertedBrain/mouse17782_teraconvert/RES(28000x45000x11786)";

//        QDir autodir("/Users/huanglei/Desktop");
//        autodir.mkdir("autoHL20220214");
//        if(!autodir.cd("autoHL20220214")){
//            qDebug()<<"cd autoHL20220214 error";
//        }else{
//            qDebug()<<autodir;
//        }

//        QDir dir("/Users/huanglei/Downloads/1708_unregistered");
//        auto files=dir.entryInfoList(QDir::Files,QDir::Name);
//        QMap<QString,QStringList> swcMap;
//        for(auto &file:files){
//            swcMap[file.baseName().split('_')[0]].push_back(file.fileName());
//        }

//        for(auto &key:swcMap.keys()){
//            QFile file(key+".txt");
//            if(file.open(QIODevice::WriteOnly)){
//                for(auto &value:swcMap[key]){
//                    QDir tmpdir=autodir;
//                    if(!tmpdir.mkdir(value.split('.').at(0)+"skip")){
//                        qDebug()<<tmpdir;
//                    }

//                    if(!QFile::copy("/Users/huanglei/Downloads/1708_unregistered/"+value,"/Users/huanglei/Desktop/autoHL20220214/"+value.split('.').at(0)+"skip/"+value)){
//                        qDebug()<<"error "<<value;
//                    }
//                    QString data=QString("C:/Users/SEU/Desktop/auto/v3d_qt6.exe %1 C:/Users/SEU/Desktop/3.603c D:/%2 D:/%2/%3\n")
//                            .arg(imageMap[key])
//                            .arg(QString("autoHL20220214/%1skip").arg(value.split('.').at(0)))
//                            .arg(value);
//                    file.write(data.toStdString().c_str());
//                }
//            }else{
//                qDebug()<<"file open failed";
//            }
//            file.close();
//        }
//    }


//    dologfile("/Users/huanglei/Desktop/18455_00152_1000.txt","/Users/huanglei/Desktop/18455_00152_1000.eswc");
//    dologfile("/Users/huanglei/Desktop/18455_00152_2000.txt","/Users/huanglei/Desktop/18455_00152_2000.eswc");
//    dologfile("/Users/huanglei/Desktop/18455_00152_3000.txt","/Users/huanglei/Desktop/18455_00152_3000.eswc");
//    dologfile("/Users/huanglei/Desktop/18455_00152_4000.txt","/Users/huanglei/Desktop/18455_00152_4000.eswc");
//    dologfile("/Users/huanglei/Desktop/18455_00152_5000.txt","/Users/huanglei/Desktop/18455_00152_5000.eswc");
//    dologfile("/Users/huanglei/Desktop/18455_00152_6000.txt","/Users/huanglei/Desktop/18455_00152_6000.eswc");
//    dologfile("/Users/huanglei/Desktop/18455_00152.txt","/Users/huanglei/Desktop/18455_00152_7000.eswc");
//    for(distthres=1;distthres<=12;++distthres)

    {
//    distthres=atoi(argv[1]);
//        //分析自动化的结果
//        auto path=QString("/Users/huanglei/Desktop/20220328/%1").arg(distthres);
//        QDir target(path);
//        auto entrylist=target.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot,QDir::Name);
//        QString out=QString("%1 %2 %3 %4 %5 %6 %7 %8 %9 %10 %11 %12 %13 %14\n")
//                .arg("distthres")
//                .arg("auto").arg("auto_length").arg("res_length")
//                .arg("d_auto_res").arg("b_auto_res")
//                .arg("d_res_auto").arg("b_res_auto")
//                .arg("auto_length/res_length").arg("b_auto_res/b_res_auto").arg("effect").arg("app2_ratio")
//                .arg("sessioncnt").arg("maxlength");
//        QFile f("/Users/huanglei/Desktop/20220328/report"+QString::number(distthres));
//        if(!f.open(QIODevice::WriteOnly))
//            qDebug()<<f.errorString();
//        f.write(out.toStdString().c_str());
//        for(auto &entry:entrylist){
//            QFile sessionfile(entry.absoluteFilePath()+"/App2_Success");
//            if(!sessionfile.open(QIODevice::ReadOnly)) continue;

//            int sessioncnt=sessionfile.readAll().trimmed().toUInt();
//            sessionfile.close();

//            QFile maxlengthfile(entry.absoluteFilePath()+"/Max_App2_Length");
//            if(!maxlengthfile.open(QIODevice::ReadOnly)) continue;

//            double maxlength=maxlengthfile.readAll().trimmed().toDouble();
//            sessionfile.close();

//            auto tmpentrylist=QDir(entry.absoluteFilePath()).entryInfoList(QDir::Files,QDir::Name);
//            QString respath="";
//            QString autopath="whole_image.eswc";
//            QString resname="";
//            for(auto &tmpentry:tmpentrylist){
//                if(!(tmpentry.absoluteFilePath().contains(".swc")
//                ||tmpentry.absoluteFilePath().contains(".eswc")))
//                    continue;
//                if(!tmpentry.baseName().contains("whole_image")){
//                    //获取答案的路径和文件名
//                    respath=tmpentry.absoluteFilePath();
//                    resname=tmpentry.baseName();
//                }else{
//                    //自动化结果的路径
//                    autopath=tmpentry.absoluteFilePath();
//                }
//            }

//            if(respath.size()==0||resname.size()==0) continue;
//            auto ana=compareA2Bv2(autopath,respath,path+"/"+resname+"skip/");

//            out=QString("%1 %2 %3 %4\n").arg(distthres).arg(ana).arg(sessioncnt).arg(maxlength);
//            f.write(out.toStdString().c_str());
//        }
    }

//    {
//        //导出csv为apo文件
//        QFile f("/Users/huanglei/Desktop/allsoma.csv");
//        f.open(QIODevice::ReadOnly);
//        QString data=f.readAll();
//        auto somainfos=data.split('\n',Qt::SkipEmptyParts);
//        QMap<QString,QList<CellAPO>> hashmap;
//        for(auto &somainfo:somainfos){
//            auto somadetail=somainfo.split(',',Qt::SkipEmptyParts);
//            CellAPO soma;
//            soma.x=somadetail[1].toFloat();
//            soma.y=somadetail[2].toFloat();
//            soma.z=somadetail[3].toFloat();
//            soma.color.r=0;
//            soma.color.g=240;
//            soma.color.b=0;
//            hashmap[somadetail[0]].push_back(soma);
//        }

//        for(auto key:hashmap.keys()){
//            writeAPO_file(key+".apo",hashmap[key]);
//        }
//    }

//    {
//        //提取bouton
//        for(auto &entry:QDir("/Users/huanglei/Desktop/SWC").entryInfoList(QDir::Files))
//        {
//            //获取bouton
//            auto nt=readSWC_file(entry.absoluteFilePath());
//            QList<CellAPO> butons;
//            for (auto &node:nt.listNeuron){
//                if(node.type==5){
//                    CellAPO buton;
//                    buton.x=node.x;
//                    buton.y=node.y;
//                    buton.z=node.z;
//                    buton.color.r=20;
//                    buton.color.g=128;
//                    buton.color.b=200;
//                    butons.push_back(buton);
//                    node.type=2;
//                }
//            }
//            qDebug()<<entry.baseName()<<" "<<butons.size();
//            writeAPO_file("/Users/huanglei/Desktop/SWC/apo/"+entry.baseName()+".apo",butons);
//            writeESWC_file("/Users/huanglei/Desktop/SWC/swc/"+entry.baseName()+".eswc",nt);
//        }
//    }


//    {
//        QVector<int> bb1={25441,26464,10457,11497,5321,5426};
//        QVector<int> bb2={25431,25686,4743,4998,4611,4796};
//        QVector<int> bb3={25361,25616,3476,3731,4769,4887};
//        QVector<QVector<int>> bbs={bb1,bb2,bb3};
//        //bouton分析
//        auto boutons_manual=readAPO_file("/Users/huanglei/Desktop/18454_00163_stamp_2022_03_25_18_43.ano.apo");
////        auto boutons_auto=readAPO_file("/Users/huanglei/Desktop/17302_00054.apo");

//        for(int j=0;j<3;j++){
//            qDebug()<<"j="<<j;
//            decltype (boutons_manual) stack,bouton1,bouton2;
//            for(int i=0;i<boutons_manual.size();i++){
//                if(boutons_manual[i].x>bbs[j][0]&&boutons_manual[i].x<bbs[j][1]
//                 &&boutons_manual[i].y>bbs[j][2]&&boutons_manual[i].y<bbs[j][3]
//                 &&boutons_manual[i].z>bbs[j][4]&&boutons_manual[i].z<bbs[j][5]){
//                    bouton1.push_back(boutons_manual[i]);
//                }
//            }

////            for(int i=0;i<boutons_auto.size();i++){
////                if(boutons_auto[i].x>bbs[j][0]&&boutons_auto[i].x<bbs[j][1]
////                 &&boutons_auto[i].y>bbs[j][2]&&boutons_auto[i].y<bbs[j][3]
////                 &&boutons_auto[i].z>bbs[j][4]&&boutons_auto[i].z<bbs[j][5]){
////                    bouton2.push_back(boutons_auto[i]);
////                }
////            }
////            int manualBoutonCnt=bouton1.size();
////            int autoBoutonCnt=bouton2.size();

////            for(auto &autobouton:bouton2){
////                bool flag=false;
////                for(auto &manualbouton:bouton1){
////                    if(abs(autobouton.x-manualbouton.x)<1&&abs(autobouton.y-manualbouton.y)<1&&abs(autobouton.z-manualbouton.z)<1){
////                        flag=true;//找到相邻的点
////                        break;
////                    }
////                }

////                if(!flag){
////                    autobouton.color.r=255;//黄色是自动化错误的点
////                    autobouton.color.g=255;//绿色是自动化正确的点
////                    autobouton.color.b=0;//蓝色是自动化没发现的点
////                    stack.push_back(autobouton);
////                }
////            }

//            int FP=0,TP=0,FN=0;
//            bouton1.append(stack);
//            for(auto &bouton:bouton1){
//                if(bouton.color.r==255&&bouton.color.g==255&&bouton.color.b==0){
//                    FP+=1;
//                }else if(bouton.color.r==0&&bouton.color.g==255&&bouton.color.b==0){
//                    TP+=1;
//                }else if(bouton.color.r==0&&bouton.color.g==0&&bouton.color.b==255){
//                    FN+=1;
//                }else{
//                    qDebug()<<"error:"<<bouton.n;
//                    qDebug()<<bouton.color.r<<" "<<bouton.color.g<<" "<<bouton.color.b;
//                }
//                bouton.n=0;
//            }

////            qDebug()<<QString("manualBoutonCnt=%1,autoBoutonCnt=%2").arg(manualBoutonCnt).arg(autoBoutonCnt);
////            qDebug()<<"stack="<<stack.size();
//            qDebug()<<QString("FP=%1,TP=%2,FN=%3").arg(FP).arg(TP).arg(FN);
////            writeAPO_file(QString("/Users/huanglei/Desktop/boutonwithanalyse%i.apo").arg(j),boutons_manual);
//        }





//    }

    return 0;
}
