#include "messageserver.h"
#include "basicdatamanage.h"
#include "ThreadPool.h"
#include "basic_c_fun/basic_surf_objs.h"
#include <QCoreApplication>
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
    {0, 0, 0}, //19 //totally black. PHC, 2012-02-15
    //the following (20-275) is used for matlab heat map. 120209 by WYN
    {0,0,131}, //20
        };

const QStringList MessageServer::clienttypes={"Terafly","TeraVR","TeraAI"};

MessageServer* MessageServer::makeMessageServer(QString neuron)
{
    Map::mutex.lock();
    auto iter=Map::NeuronMapMessageServer.find(neuron);
    if(iter!=Map::NeuronMapMessageServer.end())
    {
        Map::mutex.unlock();
        return iter.value();
    }
    label:
        qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
        QString messageport=QString::number(qrand()%1000+5000);
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
            p = new MessageServer(neuron,messageport);
//            if(!(p->listen(QHostAddress::Any,p->port.toInt())))
//                throw "";
//            else
//            {
                Map::NeuronMapMessageServer.insert(neuron,p);
//            }
        }  catch (...) {
            qDebug()<<"failed to create server";
        }
        Map::mutex.unlock();
        return p;
}

MessageServer::MessageServer(QString neuron,QString port,QObject *parent) : QTcpServer(parent)
{
    messagelist.clear();
    wholePoint.clear();
    segments.clear();
    savedMessageIndex=0;
    clients.clear();
    timer=nullptr;
    msgLog=nullptr;
    msglogstream=nullptr;

    this->neuron=neuron;
    this->port=port;
    QStringList filepaths=FE::getLoadFile(neuron);//ano,apo,eswc
    if(filepaths.isEmpty()) throw  "";
    wholePoint=readAPO_file(filepaths.at(1));
    auto NT=readSWC_file(filepaths.at(2));
    segments=NeuronTree__2__V_NeuronSWC_list(NT);

    connect(this,SIGNAL(messagecome()),this,SLOT(processmessage()));
    if(!this->listen(QHostAddress::Any,this->port.toInt()))
    {
        qDebug()<<"cannot listen messageserver in port "<<this->port;
        throw "";
    }
}


void MessageServer::incomingConnection(qintptr handle)
{
    MessageSocket* messagesocket = new MessageSocket(handle);
    QThread * thread = getNewThread();
    messagesocket->moveToThread(thread);
    QObject::connect(messagesocket,&MessageSocket::disconnected,[=]{
        clients.remove(messagesocket);
        delete messagesocket;
        releaseThread(thread);
        emit sendToAll("users:"+getUserList().join(";"));
        if(clients.size()==0) {
            /**
             * 协作结束，关闭该服务器，保存文件，释放招用端口号
             */
            save();
            Map::NeuronMapMessageServer.remove(this->neuron);
            qDebug()<<this->neuron<<" has been delete ";
            this->deleteLater();
        }
    });

    connect(this,SIGNAL(sendToAll(const QString &)),messagesocket,SLOT(sendMsg(const QString &)));
    connect(this,SIGNAL(sendfiles(MessageSocket*,QStringList)),messagesocket,SLOT(sendfiles(MessageSocket*,QStringList)));
    connect(this,SIGNAL(sendmsgs(MessageSocket*,QStringList)),messagesocket,SLOT(sendmsgs(MessageSocket*,QStringList)));
    connect(messagesocket,SIGNAL(userLogin(QString)),this,SLOT(userLogin(QString)));
    connect(messagesocket,SIGNAL(pushMsg(QString)),this,SLOT(pushMessagelist(QString)));

    thread->start();
}


void MessageServer::userLogin(QString name)
{
    auto t=autosave();
    auto p=(MessageSocket*)(sender());
    emit sendfiles(p,t.keys().at(0));
    UserInfo info;
    info.username=name;
    info.userid=getid(name);
    info.sendedsize=t.values().at(0);
    clients.insert(p,info);
    emit sendToAll("users:"+getUserList().join(";"));
    if(timer==nullptr)
    {
        timer=new QTimer();
        connect(timer,&QTimer::timeout,this,&MessageServer::autosave);
        timer->start(5*60*1000);
    }else{
        timer->stop();
        timer->start(5*60*1000);
    }
    if(!msgLog)
    {
        if(!QDir(QCoreApplication::applicationDirPath()+"/msglog").exists())
        {
            QDir(QCoreApplication::applicationDirPath()).mkdir("msglog");
        }
        msgLog=new QFile(QCoreApplication::applicationDirPath()+"/msglog/"+neuron+"txt");
        if(!msgLog->open(QIODevice::Append|QIODevice::Text|QIODevice::WriteOnly))
        {
            qDebug()<<"failed to open msglog file for "<<this->neuron;
            delete msgLog;
            msgLog=nullptr;
        }
        msglogstream=new QTextStream(msgLog);
    }
}

void MessageServer::pushMessagelist(QString msg)
{
    messagelist.push_back(msg);
    emit messagecome();
    for(auto p:clients.keys())
    {
        auto &info=clients[p];
        QStringList msgs;
        for(int i=0;i<5&&info.sendedsize<messagelist.size();info.sendedsize++)
        {
            msg.push_back(messagelist.at(info.sendedsize));
        }
        sendmsgs(p,msgs);
    }

    (*msglogstream)<<QDateTime::currentDateTimeUtc().toString("yyyy/MM/dd hh:mm:ss : ")<<msg<<endl;
}

void MessageServer::processmessage()
{
    if(savedMessageIndex!=messagelist.size())
    {
        QRegExp drawlineRex("^/drawline:(.*)$");
        QRegExp dellineRex("^/delline:(.*)");

        QRegExp addmarkerRex("^/addmarker(.*)");
        QRegExp delmarkerRex("^/delmakrer(.*)");

        QRegExp retypelineRex("^/retypeline:(.*)");
        QRegExp retypemarkerRex("^/retypemarker:(.*)");

        for(int maxProcess=0;maxProcess<5&&savedMessageIndex<messagelist.size();maxProcess++)
        {
            QString msg=messagelist[savedMessageIndex++];
            if(drawlineRex.indexIn(msg)!=-1)
            {
                drawline(drawlineRex.cap(1).trimmed());
            }else if(dellineRex.indexIn(msg)!=-1)
            {
                delline(drawlineRex.cap(1).trimmed());
            }else if(addmarkerRex.indexIn(msg)!=-1)
            {
                addmarker(drawlineRex.cap(1).trimmed());
            }else if(delmarkerRex.indexIn(msg)!=-1)
            {
                delmarekr(drawlineRex.cap(1).trimmed());
            }else if(retypelineRex.indexIn(msg)!=-1)
            {
                retypeline(drawlineRex.cap(1).trimmed());
            }else if(retypemarkerRex.indexIn(msg)!=-1)
            {
                retypemarker(drawlineRex.cap(1).trimmed());
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
    qint64 cnt=savedMessageIndex;
    QString dirPath=QCoreApplication::applicationFilePath();
    auto nt=V_NeuronSWC_list__2__NeuronTree(segments);
    QString tempAno=neuron;
    if(autosave)
    {
        QString time=QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss");
        tempAno=neuron+"_stamp_autosave_"+time;
        dirPath+="/autosave";
    }else
    {
        dirPath+="/data";
        for(int i=0;i<wholePoint.size();i++)
        {
            wholePoint[i].n=i;
        }
    }

    if(!QDir(dirPath).exists())
    {
        QDir(QCoreApplication::applicationFilePath()).mkdir(dirPath.section('/',-1));
    }

    if(!autosave)
    {
        while (savedMessageIndex!=messagelist.size()) {
            processmessage();
        }
    }

    QFile anofile(dirPath+"/"+tempAno+".ano");
    if(anofile.open(QIODevice::WriteOnly))
    {
        QTextStream out(&anofile);
        out<<"ANOFILE="+tempAno+".ano.apo"<<endl<<"SWCFILE="+tempAno+".ano.eswc";
        anofile.close();

        writeESWC_file(dirPath+"/"+tempAno+".ano.eswc",nt);
        writeAPO_file(dirPath+"/"+tempAno+".ano.apo",wholePoint);
    }
    return {  {{dirPath+"/"+tempAno+".ano",dirPath+"/"+tempAno+".ano.apo",dirPath+"/"+tempAno+".ano.eswc"},cnt}};
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
    auto it=findseg(segments.seg.begin(),segments.seg.end(),seg);

    if(it!=segments.seg.end())
    {
        segments.seg.erase(it);return;
    }

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
        marker.x=markerPara[0].toFloat();
        marker.y=markerPara[1].toFloat();
        marker.z=markerPara[2].toFloat();
        int type= markerPara[3].toInt();
        marker.color.r=neuron_type_color[type][0];
        marker.color.g=neuron_type_color[type][1];
        marker.color.b=neuron_type_color[type][2];
    }
    wholePoint.push_back(marker);
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
        float dist=distance(marker,wholePoint[i]);
        if(dist<threshold)
        {
            index=i;
        }
    }
    if(threshold<10e-0)
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
    auto it=findseg(segments.seg.begin(),segments.seg.end(),seg);

    if(it!=segments.seg.end())
    {
        for(auto & unit:it->row)
        {
            unit.type=newtype;
        }
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
    return DB::getid(username);
}

MessageServer::~MessageServer()
{
    delete timer;
    if(clients.size()!=0){
        qDebug()<<"error ,when deconstruct MessageServer there are "<<clients.size() <<" connections!";
        for(auto p:clients.keys())
        {
            p->deleteLater();
        }
    }
    msgLog->close();
    delete msgLog;
    delete msglogstream;
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
    QList<XYZ> _seg;
    QList<XYZ> _tempseg;
    const int cnt=seg.row.size();
    for(int i=0;i<cnt;i++)
    {
        _seg.push_back(seg.row.at(i));
    }

    while(begin!=end)
    {
        _tempseg.clear();
        if(begin->row.size()==cnt)
        {
            for(int i=0;i<cnt;i++)
            {
                _tempseg.push_back(begin->row.at(i));
                if(_tempseg==_seg) return begin;
                else{
                    reverse(_tempseg.begin(),_tempseg.end());
                    if(_tempseg==_seg) return begin;
                }
            }

        }
        begin++;
    }
    return end;
}






















