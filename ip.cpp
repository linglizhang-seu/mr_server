#include "basicdatamanage.h"
extern QString vaa3dPath;
extern QMap<QString,QStringList> m_MapImageIdWithRes;
extern QMap<QString,QString> m_MapImageIdWithDir;
namespace IP {
    void dirCheck(QString dirBaseName)
    {
        if(!QDir(QCoreApplication::applicationDirPath()+"/"+dirBaseName).exists())
        {
            QDir(QCoreApplication::applicationDirPath()).mkdir(dirBaseName);
        }
    }

    inline QProcess* getProcess()
    {
        return new QProcess;
    }

    inline void releaseProcess(QProcess *p)
    {
        delete p;
    }

    QString getImagePath(QString imageId,int res)
    {
        try {
            return QCoreApplication::applicationDirPath()
                    +"/image/"+m_MapImageIdWithDir.value(imageId)+"/"+m_MapImageIdWithRes.value(imageId).at(res-1);
        }  catch (...) {
            return "";
        }
    }

    int getImageRes(QString imageId)
    {
        return m_MapImageIdWithRes.value(imageId).size();
    }
    QStringList getImageBlock(QString msg)
    {
        qDebug()<<msg;
        /*
         * p1:brain_id;res;x;y;z;size;socket.descriptor
         * p2:Neuron_id/name
         * 返回：文件名，文件路径
         */
        QStringList paraList=msg.split(";",QString::SkipEmptyParts);
        QString brain_id=paraList.at(0).trimmed();//1. tf name/RES  2. .v3draw// test:17302;RES;x;y;z;b
        //0: 18465/1
        //1: 12520
        //2: 7000
        //3: 2916
        int res=paraList.at(1).toInt();//>0:1最大分辨率
        int xpos=paraList.at(2).toInt();//当前分辨率坐标
        int ypos=paraList.at(3).toInt();
        int zpos=paraList.at(4).toInt();
        int blocksize=paraList.at(5).toInt();

        dirCheck("tmp");
        QString apoName;
        {
            apoName=QCoreApplication::applicationDirPath()+"/tmp/"+paraList[6]+"__"+brain_id+"__"
                          + QString::number(xpos)+ "__"
                          + QString::number(ypos)+ "__"
                          + QString::number(zpos)+ "__"
                          + QString::number(blocksize)+"__"
                          + QString::number(blocksize)+ "__"
                          + QString::number(blocksize);
            CellAPO centerAPO;
            centerAPO.x=xpos;centerAPO.y=ypos;centerAPO.z=zpos;
            QList <CellAPO> List_APO_Write;
            List_APO_Write.push_back(centerAPO);
            if(!writeAPO_file(apoName+".apo",List_APO_Write))
            {
                qDebug()<<"fail to write apo";
                return {};//get .apo to get .v3draw
            }
        }
        QString namepart1=paraList[6]+"_"+brain_id+"_"+QString::number(blocksize)+"_";
        QString filepath;
        {
            filepath=getImagePath(brain_id,res);
            if(filepath.isEmpty()||!QFile(filepath).exists())  return {};
        }

        QString order =QString("xvfb-run -d %0/vaa3d -x %0/plugins/image_geometry/crop3d_image_series/libcropped3DImageSeries.so "
                                "-f cropTerafly -i %1/ %2.apo %3/tmp/%4 -p %5 %5 %5").arg(vaa3dPath)
                .arg(filepath).arg(apoName).arg(QCoreApplication::applicationDirPath()).arg(namepart1).arg(blocksize);
        qDebug()<<"order="<<order;
        auto p=getProcess();
        if(p->execute(order.toStdString().c_str())!=-1||p->execute(order.toStdString().c_str())>=0)
        {
            QFile f1(apoName+".apo"); qDebug()<<f1.remove();
            QString fName=namepart1+QString("%1.000_%2.000_%3.000.v3dpbd").arg(xpos).arg(ypos).arg(zpos);
            qDebug()<<fName<<"*************";releaseProcess(p);
            return {fName,QCoreApplication::applicationDirPath()+"/tmp/"+fName};
        }else
        {
            releaseProcess(p);
            return {};
        }
    }

    QStringList getSwcInBlock(const QString msg,const V_NeuronSWC_list& testVNL)
    {
        /*
         * p1:brain_id;res;x;y;z;size;socket.descriptor
         * p2:Neuron_id/name
         * 返回：文件名，文件路径
         */
        QStringList paraList=msg.split(";",QString::SkipEmptyParts);
        QString brain_id=paraList.at(0).trimmed();//1. tf name/RES  2. .v3draw// test:17302;RES;x;y;z;b
        //0: 18465/RESx18000x13000x5150
        //1: 12520
        //2: 7000
        //3: 2916
        int res=paraList.at(1).toInt();//>0
        int xpos=paraList.at(2).toInt();
        int ypos=paraList.at(3).toInt();
        int zpos=paraList.at(4).toInt();
        int blocksize=paraList.at(5).toInt();

        {
            QString name=brain_id;
            int x1=xpos-blocksize;
            int x2=xpos+blocksize;
            int y1=ypos-blocksize;
            int y2=ypos+blocksize;
            int z1=zpos-blocksize;
            int z2=zpos+blocksize;
            int cnt=pow(2,res-1);

            dirCheck("tmp");
            QString BBSWCNAME=QCoreApplication::applicationDirPath()+"/tmp/blockGet__"+name+QString("__%1__%2__%3__%4__%5__%6__%7.ano.eswc")
                    .arg(x1).arg(x2).arg(y1).arg(y2).arg(z1).arg(z2).arg(cnt);
            x1*=cnt;x2*=cnt;y1*=cnt;y2*=cnt;z1*=cnt;z2*=cnt;
            V_NeuronSWC_list tosave;
            for(int i=0;i<testVNL.seg.size();i++)
            {
                NeuronTree SS;
                V_NeuronSWC seg_temp =  testVNL.seg.at(i);
                seg_temp.reverse();
                for(int j=0;j<seg_temp.row.size();j++)
                {
                    if(seg_temp.row.at(j).x>=x1&&seg_temp.row.at(j).x<=x2
                            &&seg_temp.row.at(j).y>=y1&&seg_temp.row.at(j).y<=y2
                            &&seg_temp.row.at(j).z>=z1&&seg_temp.row.at(j).z<=z2)
                    {
                        tosave.seg.push_back(seg_temp);
                        break;
                    }
                }
            }
            qDebug()<<"get nt size:"<<tosave.seg.size();
            auto nt=V_NeuronSWC_list__2__NeuronTree(tosave);
            writeESWC_file(BBSWCNAME,nt);
            return {BBSWCNAME.right(BBSWCNAME.size()-BBSWCNAME.lastIndexOf('/')),BBSWCNAME};
        }
    }

    QStringList getApoInBlock(const QString msg,const QList <CellAPO>& wholePoint)
    {
        /*
         * p1:brain_id;res;x;y;z;size;socket.descriptor
         * p2:Neuron_id/name
         * 返回：文件名，文件路径
         */
        QStringList paraList=msg.split(";",QString::SkipEmptyParts);
        QString brain_id=paraList.at(0).trimmed();//1. tf name/RES  2. .v3draw// test:17302;RES;x;y;z;b
        //0: 18465/RESx18000x13000x5150
        //1: 12520
        //2: 7000
        //3: 2916
        int res=paraList.at(1).toInt();//>0
        int xpos=paraList.at(2).toInt();
        int ypos=paraList.at(3).toInt();
        int zpos=paraList.at(4).toInt();
        int blocksize=paraList.at(5).toInt();

        {
            QString name=brain_id;
            int x1=xpos-blocksize;
            int x2=xpos+blocksize;
            int y1=ypos-blocksize;
            int y2=ypos+blocksize;
            int z1=zpos-blocksize;
            int z2=zpos+blocksize;
            int cnt=pow(2,res-1);


            dirCheck("tmp");
            QString BBAPONAME=QCoreApplication::applicationDirPath()+"/tmp/blockGet__"+name+QString("__%1__%2__%3__%4__%5__%6__%7.ano.apo")
                    .arg(x1).arg(x2).arg(y1).arg(y2).arg(z1).arg(z2).arg(cnt);
            x1*=cnt;x2*=cnt;y1*=cnt;y2*=cnt;z1*=cnt;z2*=cnt;
            qDebug()<<"x1,x2,y1,y2,z1,z2"<<x1<<x2<<y1<<y2<<z1<<z2;
            QList <CellAPO> to_save;
            for(auto marker:wholePoint)
            {
                if(marker.x>=x1&&marker.x<=x2
                  &&marker.y>=y1&&marker.y<=y2
                  &&marker.z>=z1&&marker.z<=z2)
                {
                    to_save.append(marker);
                }
            }
            qDebug()<<"to_save.size()="<<to_save.size();
            writeAPO_file(BBAPONAME,to_save);
            return {BBAPONAME.right(BBAPONAME.size()-BBAPONAME.lastIndexOf('/')),BBAPONAME};
        }
    }
}
