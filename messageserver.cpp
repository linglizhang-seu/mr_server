#include "messageserver.h"
#include "basicdatamanage.h"
#include "ThreadPool.h"
#include "basic_c_fun/basic_surf_objs.h"
#include <QCoreApplication>
#include <cmath>
#include <QtGlobal>
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

const QStringList MessageServer::clienttypes={"TeraFly","TeraVR","TeraAI"};

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
            p = new MessageServer(neuron,messageport);
            Map::NeuronMapMessageServer.insert(neuron,p);
        }  catch (...) {
            qDebug()<<"Message:failed to create server";
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
//    qDebug()<<filepaths;
    if(filepaths.isEmpty()) throw  "";
    wholePoint=readAPO_file(filepaths.at(1));
    auto NT=readSWC_file(filepaths.at(2));
    segments=NeuronTree__2__V_NeuronSWC_list(NT);

    connect(this,SIGNAL(messagecome()),this,SLOT(processmessage()));
    if(!this->listen(QHostAddress::Any,this->port.toInt()))
    {
        qDebug()<<"cannot listen messageserver in port "<<this->port;
        throw "";
    }else
    {
        qDebug()<<"messageserver setup "<<this->neuron;
    }
}

void MessageServer::incomingConnection(qintptr handle)
{
    MessageSocket* messagesocket = new MessageSocket(handle);
    QThread * thread = getNewThread();

    QObject::connect(messagesocket,&MessageSocket::disconnected,this,[=]{
        if(clients.find(messagesocket)!=clients.end())
            clients.remove(messagesocket);
        thread->quit();
        messagesocket->deleteLater();
        releaseThread(thread);
        emit sendToAll("/users:"+getUserList().join(";"));
        if(clients.size()==0) {
            /**
             * 协作结束，关闭该服务器，保存文件，释放招用端口号
             */
            qDebug()<<"client is 0,should close "<<neuron;
            save();
            Map::NeuronMapMessageServer.remove(this->neuron);
            qDebug()<<this->neuron<<" has been delete ";
            this->deleteLater();
        }
    },Qt::DirectConnection);

    connect(this,SIGNAL(sendToAll(const QString &)),messagesocket,SLOT(sendMsg(const QString &)));
    connect(this,SIGNAL(sendfiles(MessageSocket*,QStringList)),messagesocket,SLOT(sendfiles(MessageSocket*,QStringList)));
    connect(this,SIGNAL(sendmsgs(MessageSocket*,QStringList)),messagesocket,SLOT(sendmsgs(MessageSocket*,QStringList)));
    connect(this,SIGNAL(disconnectName(MessageSocket*)),messagesocket,SLOT(disconnectName(MessageSocket*)));
    connect(messagesocket,SIGNAL(userLogin(QString)),this,SLOT(userLogin(QString)));
    connect(messagesocket,SIGNAL(pushMsg(QString)),this,SLOT(pushMessagelist(QString)));
    connect(thread,SIGNAL(started()),messagesocket,SLOT(onstarted()));
    messagesocket->moveToThread(thread);
    thread->start();
}

void MessageServer::userLogin(QString name)
{

    for(MessageSocket* key:clients.keys())
    {
        if(clients.value(key).username==name)
        {
            clients.remove(key);
            disconnectName(key);
            qDebug()<<"Error:Same Name User";
        }
    }

    auto t=autosave();
    auto p=(MessageSocket*)(sender());
    emit sendfiles(p,t.keys().at(0));
//    qDebug()<<"strp 1";
    UserInfo info;
    info.username=name;
    info.userid=getid(name);
    info.sendedsize=t.values().at(0);
    clients.insert(p,info);
//    qDebug()<<"strp 2";
    emit sendToAll("/users:"+getUserList().join(";"));
//    qDebug()<<"strp 3";
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
        msgLog=new QFile(QCoreApplication::applicationDirPath()+"/msglog/"+neuron+".txt");
        if(!msgLog->open(QIODevice::Append|QIODevice::Text|QIODevice::WriteOnly))
        {
            qDebug()<<"failed to open msglog file for "<<this->neuron;
            delete msgLog;
            msgLog=nullptr;
        }
        msglogstream=new QTextStream(msgLog);
    }
    qDebug()<<"user login end";
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

    (*msglogstream)<<QDateTime::currentDateTimeUtc().toString("yyyy/MM/dd hh:mm:ss : ")<<msg<<endl;
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
                qDebug()<<"receive:"<<++cnt;
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
    QFile anofile(dirpath+"/"+tempAno+".ano");
    qDebug()<<anofile;
    if(anofile.open(QIODevice::WriteOnly))
    {
        QTextStream out(&anofile);
        out<<"APOFILE="+tempAno+".ano.apo"<<endl<<"SWCFILE="+tempAno+".ano.eswc";
        anofile.close();

        writeESWC_file(dirpath+"/"+tempAno+".ano.eswc",nt);
        writeAPO_file(dirpath+"/"+tempAno+".ano.apo",wholePoint);
    }else
    {
        qDebug()<<anofile.errorString();
    }
    return {  {{dirpath+"/"+tempAno+".ano",dirpath+"/"+tempAno+".ano.apo",dirpath+"/"+tempAno+".ano.eswc"},cnt}};
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
    qDebug()<<segments.seg.size();
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
    qDebug()<<segments.seg.size();
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
    return DB::getid(username);
}

MessageServer::~MessageServer()
{
    delete timer;
    if(clients.size()!=0){
        qDebug()<<"error ,when deconstruct MessageServer there are "<<clients.size() <<" connections!";
        auto plist=clients.keys();

        for(auto p:plist)
        {
//            p->deleteLater();
//            clients
            clients.remove(p);
            p->socket->disconnectFromHost();
            while(p->socket->state()!=QTcpSocket::UnconnectedState)
                p->socket->waitForDisconnected();
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






















