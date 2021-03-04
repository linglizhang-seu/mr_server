#include "managesocket.h"
#include <QDataStream>
#include <QSqlQuery>
#include <QFile>
#include <QCoreApplication>
#include <QRegExp>
#include <QTime>
#include "basicdatamanage.h"
#include "messageserver.h"

void processInvite(int id)
{

}
bool ManageSocket::processMsg( const QString rmsg)
{
    if(!rmsg.endsWith('\n')) return false;
    qDebug()<<this<<"receive msg:"<<rmsg;
    QString msg=rmsg.trimmed();
    if(msg.startsWith("LOGIN:"))
    {
        QString data=msg.right(msg.size()-QString("LOGIN:").size());
        QStringList loginInfo=data.split(' ');
        //id pass
        //登陆验证
        QStringList res;
        int ret=DB::userLogin(loginInfo,res);
        res.push_front(QString::number(ret));
        sendMsg("LOGIN:"+res.join(";;"));
        if(ret)
            socket->disconnectFromHost();
    }else if(msg.startsWith("REGISTER:"))
    {
        QString data=msg.right(msg.size()-QString("REGISTER:").size());
        QStringList registerInfo=data.split(' ');
        //注册验证
        //id pass name
        sendMsg("REGISTER:"+QString::number(DB::userRegister(registerInfo)));


    }else if(msg.startsWith("FORGETPASSWORD:"))
    {
        QString data=msg.right(msg.size()-QString("FORGETPASSWORD:").size());
        //密码找回验证
        //id
        QStringList res;
        int ret=DB::findPassword(data,res);
        //发送密码邮件
    }else if(msg.startsWith("GETFILELIST:"))
    {
        QString conPath=msg.right(msg.size()-QString("GETFILELIST:").size());
        sendMsg("GETFILELIST:"+FE::getFileList(conPath));
    }else if(msg.startsWith("DOWNLOAD:"))
    {
        QStringList conPaths=msg.right(msg.size()-QString("DOWNLOAD:").size()).split(";;");
        for(auto &conPath:conPaths)
        {
            conPath=conPath.right(conPath.size()-1);
        }
        auto pathMapName=FE::getFilesPathFormFileName(conPaths.join(';'));
        sendFiles(pathMapName.firstKey(),pathMapName.value(pathMapName.firstKey()));

        //获取conpath文件夹下文件列表
        //conpath("/....")
    }else if(msg.startsWith("LOADFILES:"))
    {
        //LOADFILES:0 ****.ano ****.ano
        QStringList infos=msg.right(msg.size()-QString("LOADFILES:").size()).split(" ");
        int type=infos[0].toUInt();
        //加载
        //conpath type
        /**
         * @brief loadFiles
         * @param 2:0 从零开始，1 继承新开始， 2继承
         */

        QString conPath=infos[1];
        if(type==0)
        {
            int ix=conPath.indexOf("_x");
            int iy=conPath.indexOf("_y");
            int iz=conPath.indexOf("_z");
            int idot=conPath.indexOf(".ano");
            double x=conPath.left(iy).right(iy-ix-2).toDouble();
            double y=conPath.left(iz).right(iz-iy-2).toDouble();
            double z=conPath.left(idot).right(idot-iz-2).toDouble();
            qDebug()<<x<<" "<<y<<" "<<z;
            {
                QString anoName=infos[2];
                QString apoName=infos[2]+".apo";
                QString swcName=infos[2]+".eswc";

                NeuronTree nt;
                QList<CellAPO> cells;
                CellAPO cell;
                cell.x=x;cell.y=y;cell.z=z;
                qDebug()<<x<<" "<<y<<" "<<z;
                cells.push_back(cell);
                QFile anofile(QCoreApplication::applicationDirPath()+"/data"+anoName);
                anofile.open(QIODevice::WriteOnly);
                QString str1="APOFILE="+apoName.section("/",-1,-1);
                QString str2="SWCFILE="+swcName.section("/",-1,-1);
                QTextStream out(&anofile);
                out<<str1<<endl<<str2;
                anofile.close();

                writeESWC_file(QCoreApplication::applicationDirPath()+"/data"+swcName,nt);
                writeAPO_file(QCoreApplication::applicationDirPath()+"/data"+apoName,cells);
            }
            auto p=makeMessageServer(infos[2]);
            auto port=p?p->port:"-1";
            sendMsg("Port:"+port);
            qDebug()<<"endlllll";
        }else if(type==1)
        {
            {
                QString anoName=infos[2];
                QString apoName=infos[2]+".apo";
                QString swcName=infos[2]+".eswc";

                NeuronTree nt=readSWC_file(QCoreApplication::applicationDirPath()+"/data"+infos[1]+".eswc");
                QList<CellAPO> cells=readAPO_file(QCoreApplication::applicationDirPath()+"/data"+infos[1]+".apo");

                QFile anofile(QCoreApplication::applicationDirPath()+"/data"+anoName);
                anofile.open(QIODevice::WriteOnly);
                QString str1="APOFILE="+apoName.section("/",-1,-1);
                QString str2="SWCFILE="+swcName.section("/",-1,-1);
                QTextStream out(&anofile);
                out<<str1<<endl<<str2;
                anofile.close();

                writeESWC_file(swcName,nt);
                writeAPO_file(apoName,cells);
            }
            auto p=makeMessageServer(infos[2]);
            auto port=p?p->port:"-1";
            sendMsg("Port:"+port);
        }else if(type==2)
        {
            qDebug()<<infos[1];
            auto p=makeMessageServer(infos[1]);
            auto port=p?p->port:"-1";
            sendMsg("Port:"+port);
        }else
        {
//            auto p=makeMessageServer(infos[1].section('/',-1,-1).section('.',0));
//            auto port=p?p->port:"-1";
//            sendMsg(port+":Port");
        }
    }else if("MUSICLIST")
    {
        sendMsg("MUSICLIST:"+QDir(QCoreApplication::applicationDirPath()+"/resource/music").entryList().join(" "));
    }
    else if(msg.startsWith("GETMUSIC:"))
    {
        QStringList infos=msg.right(msg.size()-QString("LOADFILES:").size()).split(" ");
        QStringList paths;
        for(auto info:infos)
        {
            paths.push_back(QCoreApplication::applicationDirPath()+"/resource/music/"+info);
        }
        sendFiles(paths,infos);
    }
    else if(msg.startsWith("GETALLACTIVECollABORATE"))
    {
        //获取当前所有的协作列表
//        Map::mutex.lock();
//        auto mp=Map::NeuronMapMessageServer;
//        QStringList res;
//        for(auto it=mp.begin();it!=mp.end();it++)
//        {
//            res.push_back(it.key()+" "+it.value()->clients.values().first().username);
//        }

    }else{

    }
    return true;

}

bool ManageSocket::processFile( const QString filePath)
{
    return FE::processFileFromClient({filePath});
}

MessageServer* ManageSocket::makeMessageServer(QString neuron)
{
    return MessageServer::makeMessageServer(neuron);
}
