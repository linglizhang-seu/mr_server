#ifndef SIMCLIENT_H
#define SIMCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QProcess>
#include <basic_c_fun/basic_surf_objs.h>
#include <neuron_editing/neuron_format_converter.h>

class SimClient:public QObject
{
    Q_OBJECT
public:
    SimClient(QString ip,QString port,QString id,XYZ startPoint):ip(ip),port(port),id(id),ImageStartPoint(startPoint)
    {
        socket=nullptr;
        ImageMaxRes=ImageCurRes=XYZ(1);
    }
    static void cropImageAndRename(int blocksize,QString path)
    {
        QProcess p;
        QDir dir("./");
        dir.mkdir("testV3draw");dir.cd("testV3draw");dir.removeRecursively();dir.cd("../");
        dir.mkdir("data");dir.cd("data");dir.removeRecursively();dir.cd("../");

        qDebug()<<p.execute("C:/cmy_test/vaa3d_msvc.exe",QStringList()<<"/x"<<"C:/cmy_test/plugins/image_geometry/crop3d_image_series/cropped3DImageSeries.dll"
                  <<"/f"<<"cropTerafly"<<"/i"<<path<<"V3APO.apo"<<"./testV3draw/"
                  <<"/p"<<QString::number(blocksize)<<QString::number(blocksize)<<QString::number(blocksize));
        dir.cd("testV3draw");

        QFileInfoList file_list=dir.entryInfoList(QDir::Files|QDir::NoDotAndDotDot);
        QFile f("center.txt");
        if(f.open(QIODevice::WriteOnly|QIODevice::Text))
        {
            for(int i=0;i<file_list.size();i++)
            {
                QString filebasename=file_list[i].baseName();
                dir.mkdir(QString("../data/%1").arg(i+1));
                QFile(file_list[i].absoluteFilePath()).copy(QString("../data/%1/%1.v3draw").arg(i+1));
                f.write(QString(filebasename+"\n").toStdString().c_str());
            }
        }
        f.close();

    }

public slots:
    void onstarted()
    {
        socket=new QTcpSocket;
        socket->connectToHost(ip,port.toInt());
        if(socket->waitForConnected())
        {
            sendMsg(QString("/login:" +id));;
            QString swcName=autoTrace(id);
            auto nt=readSWC_file(swcName);
            auto segs=NeuronTree__2__V_NeuronSWC_list(nt);
            for(auto &seg:segs.seg)
            {
                UpdateAddSegMsg(seg,"TeraVR");
            }
        }
    }


private:
    QString autoTrace(QString para)
    {
        QProcess p;
        qDebug()<<p.execute("C:/cmy_test/vaa3d_msvc.exe", QStringList()<<"/x"<<"C:/cmy_test/plugins/neuron_tracing/Vaa3D_Neuron2/vn2.dll"
                  <<"/f"<<"app2"<<"/i"<< QString("./%1/%1.v3draw").arg(id) <<"/p"<<QString("./%1/%1.marker").arg(id)<<QString::number(0)<<QString::number(-1));
        QFileInfoList file_list=QDir(QString("./%1").arg(id)).entryInfoList(QDir::Files|QDir::NoDotAndDotDot);
        QRegExp APP2Exp("(.*)_app2.swc");
         for(int i=0;i<file_list.size();i++)
         {
             if(APP2Exp.indexIn(file_list.at(i).fileName())!=-1&&file_list.at(i).fileName().contains(QString("%1.v3draw").arg(id)))
             {
                 return file_list[i].absoluteFilePath();
             }
         }
         return "";
    }
    void sendMsg(QString str)
    {
        if(socket->state()==QAbstractSocket::ConnectedState)
        {
            const QString data=str+"\n";
            int datalength=data.size();
            QString header=QString("DataTypeWithSize:%1;;%2\n").arg(0).arg(datalength);
            socket->write(header.toStdString().c_str(),header.size());
            socket->write(data.toStdString().c_str(),data.size());
            socket->flush();
        }
    }

    void UpdateAddSegMsg(V_NeuronSWC seg,QString clienttype)
    {
        if(clienttype=="TeraFly")
        {
            QStringList result;
            result.push_back(QString("%1 %2 %3 %4 %5").arg(id).arg(clienttype).arg(ImageCurRes.x).arg(ImageCurRes.y).arg(ImageCurRes.z));
            result+=V_NeuronSWCToSendMSG(seg);
            sendMsg(QString("/drawline_norm:"+result.join(";")));
        }
    }
    QStringList V_NeuronSWCToSendMSG(V_NeuronSWC seg)
    {
        QStringList result;
        for(int i=0;i<seg.row.size();i++)   //why  i need  < 120, does msg has length limitation? liqi 2019/10/7
        {
            V_NeuronSWC_unit curSWCunit = seg.row[i];
            XYZ GlobalCroods = ConvertLocalBlocktoGlobalCroods(curSWCunit.x,curSWCunit.y,curSWCunit.z);
            result.push_back(QString("%1 %2 %3 %4").arg(curSWCunit.type).arg(GlobalCroods.x).arg(GlobalCroods.y).arg(GlobalCroods.z));
    //        if(i==seg.row.size()-1)
    //            AutoTraceNode=XYZ(GlobalCroods.x,GlobalCroods.y,GlobalCroods.z);
        }
        return result;
    }

    XYZ ConvertLocalBlocktoGlobalCroods(double x,double y,double z)
    {
        x+=(ImageStartPoint.x-1);
        y+=(ImageStartPoint.y-1);
        z+=(ImageStartPoint.z-1);
        XYZ node=ConvertCurrRes2MaxResCoords(x,y,z);
    //    qDebug()<<"ConvertLocalBlocktoGlobalCroods x y z = "<<x<<" "<<y<<" "<<z<<" -> "+XYZ2String(node);
        return node;
    }

    XYZ ConvertCurrRes2MaxResCoords(double x,double y,double z)
    {
        x*=(ImageMaxRes.x/ImageCurRes.x);
        y*=(ImageMaxRes.y/ImageCurRes.y);
        z*=(ImageMaxRes.z/ImageCurRes.z);
        return XYZ(x,y,z);
    }
    QTcpSocket *socket;
    QString ip;
    QString port;
    QString id;

    XYZ ImageMaxRes;//
    XYZ ImageCurRes;
    XYZ ImageStartPoint;
};

#endif // SIMCLIENT_H
