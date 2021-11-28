#include "messageserver.h"
#include "basic_c_fun/basic_surf_objs.h"
#include <QCoreApplication>
#include <cmath>
#include <QtGlobal>
#include <iostream>
#include <QString>
#include <QSqlDatabase>

namespace Map {
    QMap<QString,MessageServer*> NeuronMapMessageServer;
    QMutex mutex;
};
const int colorsize=21;
const int neuron_type_color[colorsize][3] = {
        {255, 255, 255},  // white,   0-undefined
        {20,  20,  20 },  // black,   1-soma
        {200, 20,  0  },  // red,     2-axon
        {0,   20,  200},  // blue,    3-dendrite
        {200, 0,   200},  // purple,  4-apical dendrite
        //the following is Hanchuan's extended color. 090331
        {0,   200, 200},  // cyan,    5
        {220, 200, 0  },  // yellow,  6
        {0,   200, 20 },  // green,   7
        {188, 94,  37 },  // coffee,  8
        {180, 200, 120},  // asparagus,	9
        {250, 100, 120},  // salmon,	10
        {120, 200, 200},  // ice,		11
        {100, 120, 200},  // orchid,	12
    //the following is Hanchuan's further extended color. 111003
    {255, 128, 168},  //	13
    {128, 255, 168},  //	14
    {128, 168, 255},  //	15
    {168, 255, 128},  //	16
    {255, 168, 128},  //	17
    {168, 128, 255}, //	18
        };
const QStringList MessageServer::clienttypes={"TeraFly","TeraVR","TeraAI","HI5"};


MessageServer::MessageServer(QString neuron,QString port,QThread *pthread,QObject *parent) : QTcpServer(parent)
{

}

void MessageServer::incomingConnection(qintptr handle)
{
    MessageSocket* messagesocket = new MessageSocket(handle);

//    QObject::connect(messagesocket,&TcpSocket::tcpdisconnected,this,MessageServer::sockDisconnect,Qt::DirectConnection);
    connect(messagesocket,&TcpSocket::tcpdisconnected,this,&MessageServer::sockDisconnect);
//    connect(shutdownTimer,&QTimer::timeout,this,&MessageServer::shutdown);/
    connect(this,SIGNAL(sendToAll(const QString &)),messagesocket,SLOT(slotSendMsg(const QString &)),Qt::DirectConnection);
    connect(this,SIGNAL(sendfiles(MessageSocket*,QStringList)),messagesocket,SLOT(sendfiles(MessageSocket*,QStringList)),Qt::DirectConnection);
    connect(this,SIGNAL(sendmsgs(MessageSocket*,QStringList)),messagesocket,SLOT(sendmsgs(MessageSocket*,QStringList)),Qt::DirectConnection);
    connect(this,SIGNAL(disconnectName(MessageSocket*)),messagesocket,SLOT(disconnectName(MessageSocket*)),Qt::DirectConnection);
    connect(messagesocket,SIGNAL(userLogin(QString)),this,SLOT(userLogin(QString)));
    connect(messagesocket,SIGNAL(pushMsg(QString)),this,SLOT(pushMessagelist(QString)));
    connect(messagesocket,SIGNAL(getBBSWC(QString)),this,SLOT(getBBSWC(QString)));
    connect(messagesocket,&MessageSocket::getscore,this,&MessageServer::getScore);
    connect(messagesocket,SIGNAL(setscore(int)),this,SLOT(setscore(int)));
    connect(messagesocket,SIGNAL(ackNeuron(QString)),this,SLOT(ackNeuron(QString)));
}

void MessageServer::userLogin(QString name)
{

}

void MessageServer::pushMessagelist(QString msg)
{
}
void MessageServer::processmessage()
}
QMap<QStringList,qint64> MessageServer::autosave()
{
    return save(1);
}
QMap<QStringList,qint64> MessageServer::save(bool autosave/*=0*/)
{

    qint64 cnt=savedMessageIndex;

    auto nt=V_NeuronSWC_list__2__NeuronTree(segments);
    QString tempAno=neuron;
    QString dirpath=QCoreApplication::applicationDirPath()+"/data";

    if(!QDir(dirpath).exists())
    {
       qDebug()<< QDir(QCoreApplication::applicationDirPath()).mkdir("data");
    }
    if(!autosave)
    {
        while (savedMessageIndex!=messagelist.size()) {
            processmessage();
        }
        for(int i=0;i<wholePoint.size();i++)
        {
            wholePoint[i].n=i;
        }
    }

    QFile anofile(dirpath+tempAno);
    if(anofile.open(QIODevice::WriteOnly))
    {
        QTextStream out(&anofile);

        out<<QString("APOFILE="+tempAno.section('/',-1)+".apo")<<endl<<QString("SWCFILE="+tempAno.section('/',-1)+".eswc");

        anofile.close();

        writeESWC_file(dirpath+tempAno+".eswc",nt);
        writeAPO_file(dirpath+tempAno+".apo",wholePoint);
    }else
    {
        qDebug()<<anofile.errorString();
    }
    return {  {{dirpath+tempAno,dirpath+tempAno+".apo",dirpath+tempAno+".eswc"},cnt}};
}
QStringList MessageServer::getUserList()
{
    auto infos=clients.values();
    QStringList usernames;
    for(auto info:infos)
        usernames.push_back(info.username);
    return usernames;
}
void MessageServer::drawline(QString msg)
{
    //line msg format:username clienttype RESx RESy RESz;type x y z;type x y z;...
    QStringList listwithheader=msg.split(';',QString::SkipEmptyParts);
    if(listwithheader.size()<=1)
    {
        qDebug()<<"msg only contains header:"<<msg;
        return;
    }
    int from=0;
    QString username;

    {
        auto headerlist=listwithheader[0].split(' ',QString::SkipEmptyParts);
        QString clienttype=headerlist[1].trimmed();
        for(int i=0;i<clienttypes.size();i++)
        {
            if(clienttypes[i]==clienttype)
            {
                from = i;
                break;
            }
        }
        username=headerlist[0].trimmed();
    }

    NeuronTree newTempNT=convertMsg2NT(listwithheader,username,from);
    segments.append(NeuronTree__2__V_NeuronSWC_list(newTempNT).seg[0]);
//    qDebug()<<"add in seg sucess "<<msg;
}
void MessageServer::delline(QString msg)
{
   //line msg format:username clienttype RESx RESy RESz;type x y z;type x y z;...
    QStringList listwithheader=msg.split(';',QString::SkipEmptyParts);
    if(listwithheader.size()<=1)
    {
        qDebug()<<"msg only contains header:"<<msg;
        return;
    }

    NeuronTree newTempNT;

    newTempNT=convertMsg2NT(listwithheader);
    auto seg=NeuronTree__2__V_NeuronSWC_list(newTempNT).seg[0];
//    qDebug()<<segments.seg.size();
    auto it=findseg(segments.seg.begin(),segments.seg.end(),seg);

    if(it!=segments.seg.end())
    {
        segments.seg.erase(it);
//        qDebug()<<"find delete line sucess"<<msg;
    }else
        qDebug()<<"not find delete line "<<msg;
}
void MessageServer::addmarker(QString msg)
{
    //marker msg format:username clienttype RESx RESy RESz;type x y z
    QStringList listwithheader=msg.split(';',QString::SkipEmptyParts);
    if(listwithheader.size()<=1)
    {
        qDebug()<<"msg only contains header:"<<msg;
        return;
    }

    CellAPO marker;
    {
        QStringList markerPara=listwithheader[1].split(' ',QString::SkipEmptyParts);
        marker.x=markerPara[1].toFloat();
        marker.y=markerPara[2].toFloat();
        marker.z=markerPara[3].toFloat();
        int type= markerPara[0].toInt();
        marker.color.r=neuron_type_color[type][0];
        marker.color.g=neuron_type_color[type][1];
        marker.color.b=neuron_type_color[type][2];
    }
    wholePoint.push_back(marker);
//    qDebug()<<"add in seg marker "<<msg;
}
void MessageServer::delmarekr(QString msg)
{
    //marker msg format:username clienttype RESx RESy RESz;type x y z
    QStringList listwithheader=msg.split(';',QString::SkipEmptyParts);
    if(listwithheader.size()<=1)
    {
        qDebug()<<"msg only contains header:"<<msg;
        return;
    }

    CellAPO marker;
    {
        QStringList markerPara=listwithheader[1].split(' ',QString::SkipEmptyParts);
        marker.x=markerPara[1].toFloat();
        marker.y=markerPara[2].toFloat();
        marker.z=markerPara[3].toFloat();
    }
    int index=-1;
    double threshold=10e-0;
    for(int i=0;i<wholePoint.size();i++)
    {
        double dist=distance(marker,wholePoint[i]);
        if(dist<threshold)
        {
            index=i;
        }
    }
    if(index>=0)
    {
//        qDebug()<<"delete marker:"<<wholePoint[index].x<<" "<<wholePoint[index].y<<" "<<wholePoint[index].z
//               <<",msg = "<<msg;
       wholePoint.removeAt(index);
    }else
    {
        qDebug()<<"failed to find marker to delete ,msg = "<<msg;
    }

}
void MessageServer::retypeline(QString msg)
{
    //line msg format:username clienttype  newtype RESx RESy RESz;type x y z;type x y z;...
    QStringList listwithheader=msg.split(';',QString::SkipEmptyParts);
    if(listwithheader.size()<=1)
    {
        qDebug()<<"msg only contains header:"<<msg;
        return;
    }


    int newtype=listwithheader[0].split(' ',QString::SkipEmptyParts)[2].trimmed().toUInt();
    if(!(newtype<colorsize)) newtype=defaulttype;
    NeuronTree newTempNT;

    newTempNT=convertMsg2NT(listwithheader);
    auto seg=NeuronTree__2__V_NeuronSWC_list(newTempNT).seg[0];
//    qDebug()<<segments.seg.size();
    auto it=findseg(segments.seg.begin(),segments.seg.end(),seg);

    if(it!=segments.seg.end())
    {
        for(auto & unit:it->row)
        {
            unit.type=newtype;
        }
//        qDebug()<<"find retype line sucess "<<msg;
        return;
    }
    qDebug()<<"not find retype line "<<msg;

}
void MessageServer::retypemarker(QString msg)
{
    //marker msg format:username clienttype newtype RESx RESy RESz;type x y z
    QStringList listwithheader=msg.split(';',QString::SkipEmptyParts);
    if(listwithheader.size()<=1)
    {
        qDebug()<<"msg only contains header:"<<msg;
        return;
    }
    int newtype=listwithheader[0].split(' ',QString::SkipEmptyParts)[2].trimmed().toUInt();
    if(!(newtype<colorsize)) newtype=defaulttype;
    CellAPO marker;
    {
        QStringList markerPara=listwithheader[1].split(' ',QString::SkipEmptyParts);
        marker.x=markerPara[1].toFloat();
        marker.y=markerPara[2].toFloat();
        marker.z=markerPara[3].toFloat();
    }

    int index=-1;
    double threshold=10e-0;
    for(int i=0;i<wholePoint.size();i++)
    {
        float dist=distance(marker,wholePoint[i]);
        if(dist<threshold)
        {
            index=i;
        }
    }

    if(threshold<ths)
    {
        qDebug()<<"retype marker:"<<wholePoint[index].x<<" "<<wholePoint[index].y<<" "<<wholePoint[index].z
               <<",msg = "<<msg;
        wholePoint[index].color.r=neuron_type_color[newtype][0];
        wholePoint[index].color.g=neuron_type_color[newtype][1];
        wholePoint[index].color.b=neuron_type_color[newtype][2];
    }else
    {
        qDebug()<<"failed to find marker to delete ,msg = "<<msg;
    }
}

int MessageServer::getid(QString username)
{
//    return username.toUInt();
    static QMap<QString,int> name_id;
    if(!name_id.contains(username))
        name_id[username]=DB::getid(db,username);
    return name_id[username];
}

MessageServer::~MessageServer()
{
    delete timer;
    delete shutdownTimer;
    db.close();
    QSqlDatabase::removeDatabase(port+neuron);
    Map::NeuronMapMessageServer.remove(this->neuron);
    timer=0;
    shutdownTimer=0;
    if(clients.size()!=0){
        qDebug()<<"error ,when deconstruct MessageServer there are "<<clients.size() <<" connections!";
        auto plist=clients.keys();

        for(auto p:plist)
        {
            clients.remove(p);
            p->socket->disconnectFromHost();
//            while(p->socket->state()!=QTcpSocket::UnconnectedState)
//                p->socket->waitForDisconnected();
        }
    }
}

NeuronTree MessageServer::convertMsg2NT(QStringList &listwithheader,QString username,int from)
{
    NeuronTree newTempNT;
    newTempNT.listNeuron.clear();
    newTempNT.hashNeuron.clear();
    int cnt=listwithheader.size();
    int type=-1;

    for(int i=1;i<cnt;i++)
    {
        NeuronSWC S;
        QStringList nodelist=listwithheader[i].split(' ',QString::SkipEmptyParts);
        if(nodelist.size()<4) return NeuronTree();
        S.n=i;
        type=nodelist[0].toUInt();
        if(type<colorsize)
            S.type=type;
        else
            S.type=defaulttype;
        S.x=nodelist[1].toFloat();
        S.y=nodelist[2].toFloat();
        S.z=nodelist[3].toFloat();
        S.r=getid(username)*10+from;
        if(i==1) S.pn=-1;
        else S.pn=i-1;

        newTempNT.listNeuron.push_back(S);
        newTempNT.hashNeuron.insert(S.n,newTempNT.listNeuron.size());
    }
    return newTempNT;
}

vector<V_NeuronSWC>::iterator MessageServer::findseg(vector<V_NeuronSWC>::iterator begin,vector<V_NeuronSWC>::iterator end,const V_NeuronSWC seg)
{

    vector<V_NeuronSWC>::iterator result=end;
    double mindist=0.2;
    const int cnt=seg.row.size();

    while(begin!=end)
    {
        if(begin->row.size()==cnt)
        {
            double dist=0;
            for(int i=0;i<cnt;i++)
            {
                auto node=begin->row.at(i);
                dist+=sqrt(
                           pow(node.x-seg.row[i].x,2)
                          +pow(node.y-seg.row[i].y,2)
                          +pow(node.z-seg.row[i].z,2)
                           );
            }
            if(dist/cnt<mindist)
            {
                mindist=dist;
                result=begin;
            }

            dist=0;
            for(int i=0;i<cnt;i++)
            {
                auto node=begin->row.at(i);
                dist+=sqrt(
                           pow(node.x-seg.row[cnt-i-1].x,2)
                          +pow(node.y-seg.row[cnt-i-1].y,2)
                          +pow(node.z-seg.row[cnt-i-1].z,2)
                           );
            }
            if(dist/cnt<mindist)
            {
                mindist=dist;
                result=begin;
            }
        }
        begin++;
    }
    return result;
}

void MessageServer::getBBSWC(QString paraStr)
{
    if(paraStr.isEmpty())
    {
        auto t=autosave();
        auto p=(MessageSocket*)(sender());
        emit sendfiles(p,t.keys().at(0));
        clients[p].sendedsize=savedMessageIndex;
    }
    else
    {
        auto swc_name_path=IP::getSwcInBlock(paraStr,segments);
        auto apo_name_path=IP::getApoInBlock(paraStr,wholePoint);
        emit sendfiles((MessageSocket*)(sender()),{swc_name_path.at(1),apo_name_path.at(1)});
        clients[(MessageSocket*)(sender())].sendedsize=savedMessageIndex;
    }
}

void MessageServer::setscore(int s)
{
    MessageSocket *kp=(MessageSocket*)sender();
    if(clients.contains(kp))
    {
        clients[kp].score=s;
    }
}

void MessageServer::sockDisconnect()
{
    MessageSocket* p=(MessageSocket*)(sender());
    if(!clients.remove(p))
        qDebug()<<"Confirm:messagesocket not in clients";

    if(clients.size()==0)
    {
        shutdownTimer->start(60*1000);
//        shutdown();
    }else {
        emit sendToAll("/users:"+getUserList().join(";"));
    }
    p->deleteLater();
}

void MessageServer::shutdown()
{
    if(clients.size())
    {
        shutdownTimer->stop();
    }
    else
    {
        qDebug()<<"client is 0,should close "<<neuron;
        save();
        qDebug()<<"save success";

        qDebug()<<this->neuron<<" has been delete ";
        p_thread->quit();
        p_thread=nullptr;
    }
}

void MessageServer::getScore()
{
    MessageSocket *kp=(MessageSocket*)sender();
    if(clients.contains(kp))
    {
       emit  sendmsgs(kp,{QString("Score:%1 %2").arg(clients[kp].username).arg(clients[kp].score)});
    }else
        emit  sendmsgs(kp,{QString("Please login before getscores!")});
}

void MessageServer::ackNeuron(QString id)
{
    DB::setAck(db,id);
}

void MessageServer::onstarted()
{
    shutdownTimer=new QTimer;
    connect(shutdownTimer,&QTimer::timeout,this,&MessageServer::shutdown,Qt::DirectConnection);


    {
        if(!QDir(QCoreApplication::applicationDirPath()+"/data"+neuron.section("/",0,-2)+"/msglog").exists())
        {
            QDir(QCoreApplication::applicationDirPath()+"/data"+neuron.section("/",0,-2)).mkdir("msglog");
        }
        msgLog.setFileName(QCoreApplication::applicationDirPath()+"/data"+neuron.section("/",0,-2)+"/msglog/"+neuron.section('/',-1,-1)+".txt");
        if(!msgLog.open(QIODevice::Append|QIODevice::Text|QIODevice::WriteOnly))
        {
            qDebug()<<"failed to open msglog file for "<<this->neuron<<msgLog.errorString();
        }else
        msglogstream.setDevice(&msgLog);
    }
    qDebug()<<"Set up databse :"<<port+neuron;

    db=QSqlDatabase::addDatabase("QMYSQL",port+neuron);
    db.setDatabaseName(databaseName);
    db.setHostName(dbHostName);
    db.setUserName(dbUserName);
    db.setPassword(dbPassword);
    qDebug()<<"Set up databse :"<<port+neuron<<" end";
}













