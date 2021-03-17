#include "messageserver.h"
#include "basicdatamanage.h"
#include "ThreadPool.h"
#include "basic_c_fun/basic_surf_objs.h"
#include <QCoreApplication>
#include <cmath>
#include <QtGlobal>
#include <iostream>
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

MessageServer* MessageServer::makeMessageServer(QString neuron)
{
    //neuron:/17301/17301_00019/*****.ano
    Map::mutex.lock();
    auto iter=Map::NeuronMapMessageServer.find(neuron);
    if(iter!=Map::NeuronMapMessageServer.end())
    {
        Map::mutex.unlock();
        return iter.value();
    }
    label:
        qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
        QString messageport=QString::number(qrand()%1000+4000);
        QStringList keys=Map::NeuronMapMessageServer.keys();
        for(QString neuron:keys)
        {
            if(Map::NeuronMapMessageServer.value(neuron)->port==messageport)
            {
                goto label;
            }
        }
        MessageServer* p=0;
        try {
            QThread * p_thread = getNewThread();
            p = new MessageServer(neuron,messageport,p_thread);
            Map::NeuronMapMessageServer.insert(neuron,p);
            connect(p_thread,&QThread::finished,p,&MessageServer::deleteLater);
            connect(p,SIGNAL(messagecome()),p,SLOT(processmessage()));
            connect(p_thread,SIGNAL(started()),p,SLOT(onstarted()));
            p->moveToThread(p_thread);
            p_thread->start();
            qDebug()<<"create server for "<<neuron<<" success "<<p->port;
        }  catch (...) {
            qDebug()<<"Message:failed to create server";
        }
        Map::mutex.unlock();
        return p;
}

MessageServer::MessageServer(QString neuron,QString port,QThread *pthread,QObject *parent) : QTcpServer(parent)
{
    messagelist.clear();
    wholePoint.clear();
    segments.clear();
    savedMessageIndex=0;
    clients.clear();
    timer=nullptr;
    this->neuron=neuron;
    this->port=port;
    this->p_thread=pthread;
    QStringList filepaths=FE::getLoadFile(neuron);//ano,apo,eswc
//    qDebug()<<filepaths;
    if(filepaths.isEmpty()) throw  "";
    wholePoint=readAPO_file(filepaths.at(1));
    auto NT=readSWC_file(filepaths.at(2));
    segments=NeuronTree__2__V_NeuronSWC_list(NT);

    if(!this->listen(QHostAddress::Any,this->port.toInt()))
    {
        qDebug()<<"cannot listen messageserver in port "<<this->port;
        throw "";
    }else
    {
        qDebug()<<"messageserver setup "<<this->neuron<<" "<<this->port;
    }

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

}

void MessageServer::setscores()
{
    QStringList names;
    std::vector<int> scores;
    for(auto v:clients.values())
    {
        names.push_back(v.username);
        scores.push_back(v.score);
    }
    if(DB::setScores(names,scores))
        std::cerr<<"Fatal error, write scores error"<<endl;
}

void MessageServer::incomingConnection(qintptr handle)
{
    MessageSocket* messagesocket = new MessageSocket(handle);
//    qDebug()<<"this....."<<messagesocket;
    QObject::connect(messagesocket,&TcpSocket::tcpdisconnected,this,[=]{

        setscores();
        if(!clients.remove(messagesocket))
            qDebug()<<"Confirm:messagesocket not in clients";
        delete messagesocket;

        if(clients.size()==0) {
            /**
             * 协作结束，关闭该服务器，保存文件，释放招用端口号
             */  
            QTimer::singleShot(60*1000,this,[=]{
                if(clients.size()) return;
                this->close();
                qDebug()<<"client is 0,should close "<<neuron;
                save();
                Map::NeuronMapMessageServer.remove(this->neuron);
                qDebug()<<this->neuron<<" has been delete ";
                p_thread->quit();
            });
        }else
        {
            emit sendToAll("/users:"+getUserList().join(";"));
        }
    },Qt::DirectConnection);

    connect(this,SIGNAL(sendToAll(const QString &)),messagesocket,SLOT(slotSendMsg(const QString &)),Qt::DirectConnection);
    connect(this,SIGNAL(sendfiles(MessageSocket*,QStringList)),messagesocket,SLOT(sendfiles(MessageSocket*,QStringList)),Qt::DirectConnection);
    connect(this,SIGNAL(sendmsgs(MessageSocket*,QStringList)),messagesocket,SLOT(sendmsgs(MessageSocket*,QStringList)),Qt::DirectConnection);
    connect(this,SIGNAL(disconnectName(MessageSocket*)),messagesocket,SLOT(disconnectName(MessageSocket*)),Qt::DirectConnection);
    connect(messagesocket,SIGNAL(userLogin(QString)),this,SLOT(userLogin(QString)));
    connect(messagesocket,SIGNAL(pushMsg(QString)),this,SLOT(pushMessagelist(QString)));
    connect(messagesocket,SIGNAL(getBBSWC(QString)),this,SLOT(getBBSWC(QString)));
    connect(messagesocket,&MessageSocket::getscore,this,[=]{
         MessageSocket *kp=(MessageSocket*)sender();
         if(clients.contains(kp))
         {
            emit  sendmsgs(kp,{QString("Score:%1 %2").arg(clients[kp].username).arg(clients[kp].score)});
         }else
             emit  sendmsgs(kp,{QString("Please login before getscores!")});
    });
    connect(messagesocket,SIGNAL(setscore(int)),this,SLOT(setscore(int)));
}

void MessageServer::userLogin(QString name)
{

    MessageSocket *kp=nullptr;
    {
        for(MessageSocket* key:clients.keys())
        {
            if(clients.value(key).username==name)
            {
                kp=key;
                break;
            }
        }
    }

    auto t=autosave();
    auto p=(MessageSocket*)(sender());
    UserInfo info;
    info.username=name;
    info.userid=getid(name);
    info.sendedsize=t.values().at(0);
    info.score=DB::getScore(name);
    clients.insert(p,info);
    if(kp)
    {
        qDebug()<<"find same name ,first"<<kp<<",second "<<p;
//        clients.remove(kp);
        disconnectName(kp);
    }
    emit sendToAll("/users:"+getUserList().join(";"));
    if(timer==nullptr)
    {
        timer=new QTimer();
        connect(timer,&QTimer::timeout,this,&MessageServer::autosave);
        timer->start(5*60*1000);
    }else{
        timer->stop();
        timer->start(5*60*1000);
    }
    p->username=name;
}

void MessageServer::pushMessagelist(QString msg)
{
    messagelist.push_back(msg);
    emit messagecome();
    for(auto p:clients.keys())
    {
        auto &info=clients[p];
        QStringList msgs;
        for(int i=0;i<5&&info.sendedsize<messagelist.size();info.sendedsize++,i++)
        {
            msgs.push_back(messagelist.at(info.sendedsize));
        }
        sendmsgs(p,msgs);
    }

    (msglogstream)<<QDateTime::currentDateTimeUtc().toString("yyyy/MM/dd hh:mm:ss : ")<<msg<<endl;
    msglogstream.flush();
}
void MessageServer::processmessage()
{
    if(savedMessageIndex!=messagelist.size())
    {
        QRegExp msgreg("/(.*)_(.*):(.*)");

        for(int maxProcess=0;maxProcess<5&&savedMessageIndex<messagelist.size();maxProcess++)
        {
            QString msg=messagelist[savedMessageIndex++];
            if(msgreg.indexIn(msg)!=-1)
            {
                QString operationtype=msgreg.cap(1).trimmed();
//                bool isNorm=msgreg.cap(2).trimmed()=="norm";
                QString operatorMsg=msgreg.cap(3).trimmed();
                if(operationtype == "drawline" )
                {
                    drawline(operatorMsg);
                }
                else if(operationtype == "delline")
                {
                    delline(operatorMsg);
                }
                else if(operationtype == "addmarker")
                {
                    addmarker(operatorMsg);
                }
                else if(operationtype == "delmarker")
                {
                    delmarekr(operatorMsg);
                }
                else if(operationtype == "retypeline")
                {
                    retypeline(operatorMsg);
                }
            }
        }
    }
}
QMap<QStringList,qint64> MessageServer::autosave()
{
    return save(1);
}
QMap<QStringList,qint64> MessageServer::save(bool autosave/*=0*/)
{
    setscores();
    qint64 cnt=savedMessageIndex;

    auto nt=V_NeuronSWC_list__2__NeuronTree(segments);
    QString tempAno=neuron;
    QString dirpath=QCoreApplication::applicationDirPath()+"/data";
//    qDebug()<<dirpath;
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
//    qDebug()<<anofile;
    if(anofile.open(QIODevice::WriteOnly))
    {
        QTextStream out(&anofile);
        out<<"APOFILE="+tempAno.section('/',-1)+".apo"<<endl<<"SWCFILE="+tempAno.section('/',-1)+".eswc";
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
    qDebug()<<"add in seg sucess "<<msg;
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
        qDebug()<<"find delete line sucess"<<msg;
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
    qDebug()<<"add in seg marker "<<msg;
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
        qDebug()<<"delete marker:"<<wholePoint[index].x<<" "<<wholePoint[index].y<<" "<<wholePoint[index].z
               <<",msg = "<<msg;
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
        qDebug()<<"find retype line sucess "<<msg;
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
        name_id[username]=DB::getid(username);
    return name_id[username];
}

MessageServer::~MessageServer()
{
    delete timer;
    timer=0;
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




















