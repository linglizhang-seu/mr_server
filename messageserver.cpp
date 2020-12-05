#include "messageserver.h"
#include "somefunction.h"
#include "ThreadPool.h"
#include "basic_c_fun/basic_surf_objs.h"
#include <QCoreApplication>
namespace Map {
    QMap<QString,MessageServer*> NeuronMapMessageServer;
    QMutex mutex;
};

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
            p = new MessageServer(neuron);
            p->port=messageport;
            if(!(p->listen(QHostAddress::Any,p->port.toInt())))
                throw "";
            else
            {
                Map::NeuronMapMessageServer.insert(neuron,p);
            }
        }  catch (...) {
            qDebug()<<"failed to create server";
        }
        Map::mutex.unlock();
        return p;
}


MessageServer::MessageServer(QString neuron,QObject *parent) : QTcpServer(parent)
{
    this->neuron=neuron;
    bool f=false;
    auto map=getANOFILE(neuron,f);
    if(!f)
    {
        qDebug()<<"cannot get file "<<neuron;
        throw "";
    }
    timer=new QTimer();
    connect(timer,&QTimer::timeout,this,&MessageServer::autosave);
    timer->start(5*60*1000);

    messagelist.clear();
    wholePoint.clear();
    segments.clear();
    clients.clear();

    savedMessageIndex=0;
    wholePoint=readAPO_file(map.keys().at(0).at(1));
    pointIndex=wholePoint.size();
    auto NT=readSWC_file(map.values().at(0).at(2));
    segments=NeuronTree__2__V_NeuronSWC_list(NT);
    connect(this,SIGNAL(messagecome()),this,SLOT(processmessage()));
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
        emit sendToAll("users:"+getUserList());
        if(clients.size()==0) {
            save();
            Map::NeuronMapMessageServer.remove(Map::NeuronMapMessageServer.key(this));
            this->deleteLater();
        }
    });

    connect(this,SIGNAL(sendToAll(QString)),messagesocket,SLOT(sendMsg(QString)));
    connect(this,SIGNAL(sendfiles(MessageSocket*,QStringList)),messagesocket,SLOT(sendfiles(MessageSocket*,QStringList)));
    connect(this,SIGNAL(sendmsgs(MessageSocket*,QStringList)),messagesocket,SLOT(sendmsgs(MessageSocket*,QStringList)));
    connect(messagesocket,SIGNAL(userLogin(QString)),this,SLOT(userLogin(QString)));
    connect(messagesocket,SIGNAL(pushMsg(QString,bool)),this,SLOT(pushMessagelist(QString,bool)));

    thread->start();
}

void MessageServer::pushMessagelist(QString msg,bool isAddMarker)
{
    if(isAddMarker) msg+=";"+QString::number(pointIndex++);
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

void MessageServer::userLogin(QString name)
{
    /*
     * 先自动保存文件，获取savedMessageIndex
     * 发送自动保存的文件
     * 发送savedMessageIndex
     */
    auto t=autosave();
    auto p=(MessageSocket*)(sender());
    emit sendfiles(p,t.keys().at(0));
    emit sendmsgs(p,{"InitMsgCnt:"+QString::number(t.values().at(0))});
    UserInfo info;
    info.username=name;
    info.userid=getid(name);
    info.sendedsize=t.values().at(0);
    clients.insert(p,info);
    emit sendToAll("users:"+getUserList());
}

QString MessageServer::getUserList()
{
    auto infos=clients.values();
    QStringList usernames;
    for(auto info:infos)
        usernames.push_back(info.username);
    return usernames.join(';');
}

void MessageServer::drawline(QString msg)
{
    QStringList listwithheader=msg.split(';',QString::SkipEmptyParts);
    int cnt=listwithheader.size();
    int from=0;
    QString username;
    if(cnt<=1) return;
    //
    {
        auto tmp=listwithheader[0].split(' ',QString::SkipEmptyParts);
        QString title=tmp[0].trimmed();
        username=tmp[1].trimmed();

        if(title=="Terafly") from=0;
        else if(title=="TeraVR") from=1;
        else if(title=="TeraAI") from=2;
    }

    NeuronTree newTempNT;
    newTempNT.listNeuron.clear();
    newTempNT.hashNeuron.clear();
    for(int i=1;i<cnt;i++)
    {
        NeuronSWC S;
        QStringList nodelist=listwithheader[i].split(' ',QString::SkipEmptyParts);
        if(nodelist.size()<5) return;
        S.n=i;
        S.type=nodelist[1].toInt();
        S.x=nodelist[2].toFloat();
        S.y=nodelist[3].toFloat();
        S.z=nodelist[4].toFloat();
        S.r=getid(username)*10+from;
        if(i==1) S.pn=-1;
        else S.pn=i-1;

        newTempNT.listNeuron.push_back(S);
        newTempNT.hashNeuron.insert(S.n,newTempNT.listNeuron.size());
    }
    segments.append(NeuronTree__2__V_NeuronSWC_list(newTempNT).seg[0]);
}
void MessageServer::delline(QString msg)
{
    QStringList listwithheader=msg.split(';',QString::SkipEmptyParts);
    int cnt=listwithheader.size();
    int from=0;
    QString username;
    if(cnt<=1) return;
    //
    {
        auto tmp=listwithheader[0].split(' ',QString::SkipEmptyParts);
        QString title=tmp[0].trimmed();
        username=tmp[1].trimmed();

        if(title=="Terafly") from=0;
        else if(title=="TeraVR") from=1;
        else if(title=="TeraAI") from=2;
    }

    NeuronTree newTempNT;
    newTempNT.listNeuron.clear();
    newTempNT.hashNeuron.clear();
    for(int i=1;i<cnt;i++)
    {
        NeuronSWC S;
        QStringList nodelist=listwithheader[i].split(' ',QString::SkipEmptyParts);
        if(nodelist.size()<5) return;
        S.n=i;
        S.type=nodelist[1].toInt();
        S.x=nodelist[2].toFloat();
        S.y=nodelist[3].toFloat();
        S.z=nodelist[4].toFloat();
        S.r=getid(username)*10+from;
        if(i==1) S.pn=-1;
        else S.pn=i-1;

        newTempNT.listNeuron.push_back(S);
        newTempNT.hashNeuron.insert(S.n,newTempNT.listNeuron.size());
    }
    auto seg=NeuronTree__2__V_NeuronSWC_list(newTempNT).seg[0];
    segments.seg.erase(find(segments.seg.begin(),segments.seg.end(),seg));
}
void MessageServer::addmarker(QString msg)
{
    QStringList listwithheader=msg.split(';',QString::SkipEmptyParts);
    int cnt=listwithheader.size();
    int from=0;
    QString username;
    if(cnt<=1) return;
    //
    {
        auto tmp=listwithheader[0].split(' ',QString::SkipEmptyParts);
        QString title=tmp[0].trimmed();
        username=tmp[1].trimmed();

        if(title=="Terafly") from=0;
        else if(title=="TeraVR") from=1;
        else if(title=="TeraAI") from=2;
    }

    CellAPO marker;
    {
        QStringList markerPara=listwithheader[1].split(' ',QString::SkipEmptyParts);
        marker.n=listwithheader[2].toInt();
        marker.x=markerPara[0].toFloat();
        marker.y=markerPara[1].toFloat();
        marker.z=markerPara[2].toFloat();
        marker.color.r=markerPara[3].toInt();
        marker.color.g=markerPara[4].toInt();
        marker.color.g=markerPara[5].toInt();
    }
    wholePoint.push_back(marker);
}
void MessageServer::delmarekr(QString msg)
{
    QStringList listwithheader=msg.split(';',QString::SkipEmptyParts);
    int cnt=listwithheader.size();
    int from=0;
    QString username;
    if(cnt<=1) return;
    //
    {
        auto tmp=listwithheader[0].split(' ',QString::SkipEmptyParts);
        QString title=tmp[0].trimmed();
        username=tmp[1].trimmed();

        if(title=="Terafly") from=0;
        else if(title=="TeraVR") from=1;
        else if(title=="TeraAI") from=2;
    }

    CellAPO marker;
    {
        QStringList markerPara=listwithheader[1].split(' ',QString::SkipEmptyParts);
        marker.n=markerPara[6].toInt();
        marker.x=markerPara[0].toFloat();
        marker.y=markerPara[1].toFloat();
        marker.z=markerPara[2].toFloat();
        marker.color.r=markerPara[3].toInt();
        marker.color.g=markerPara[4].toInt();
        marker.color.g=markerPara[5].toInt();
    }
    wholePoint.removeAt(wholePoint.indexOf(marker));
}
void MessageServer::retypeline(QString msg)
{
    QStringList listwithheader=msg.split(';',QString::SkipEmptyParts);
    int cnt=listwithheader.size();

    if(cnt<=1) return;
    //
    int from=0;
    QString username;
    int type=0;
    {
        auto tmp=listwithheader[0].split(' ',QString::SkipEmptyParts);
        QString title=tmp[0].trimmed();
        username=tmp[1].trimmed();

        if(title=="Terafly") from=0;
        else if(title=="TeraVR") from=1;
        else if(title=="TeraAI") from=2;
        type=tmp[2].toInt();
    }

    NeuronTree newTempNT;
    newTempNT.listNeuron.clear();
    newTempNT.hashNeuron.clear();
    for(int i=1;i<cnt;i++)
    {
        NeuronSWC S;
        QStringList nodelist=listwithheader[i].split(' ',QString::SkipEmptyParts);
        if(nodelist.size()<5) return;
        S.n=i;
        S.type=nodelist[1].toInt();
        S.x=nodelist[2].toFloat();
        S.y=nodelist[3].toFloat();
        S.z=nodelist[4].toFloat();
        S.r=getid(username)*10+from;
        if(i==1) S.pn=-1;
        else S.pn=i-1;

        newTempNT.listNeuron.push_back(S);
        newTempNT.hashNeuron.insert(S.n,newTempNT.listNeuron.size());
    }
    auto seg=NeuronTree__2__V_NeuronSWC_list(newTempNT).seg[0];
    auto it=find(segments.seg.begin(),segments.seg.end(),seg);
    for(auto &node:it->row)
    {
        node.type=type;
    }
}
void MessageServer::retypemarker(QString msg)
{
    QStringList listwithheader=msg.split(';',QString::SkipEmptyParts);
    int cnt=listwithheader.size();

    if(cnt<=1) return;
    int from=0;
    QString username;
    //
    int r,g,b;
    {
        auto tmp=listwithheader[0].split(' ',QString::SkipEmptyParts);
        QString title=tmp[0].trimmed();
        username=tmp[1].trimmed();

        if(title=="Terafly") from=0;
        else if(title=="TeraVR") from=1;
        else if(title=="TeraAI") from=2;
        r=tmp[2].toInt();r=tmp[3].toInt();r=tmp[4].toInt();
    }

    CellAPO marker;
    {
        QStringList markerPara=listwithheader[1].split(' ',QString::SkipEmptyParts);
        marker.n=markerPara[6].toInt();
        marker.x=markerPara[0].toFloat();
        marker.y=markerPara[1].toFloat();
        marker.z=markerPara[2].toFloat();
        marker.color.r=markerPara[3].toInt();
        marker.color.g=markerPara[4].toInt();
        marker.color.g=markerPara[5].toInt();
    }
    wholePoint[wholePoint.indexOf(marker)].color.r=r;
    wholePoint[wholePoint.indexOf(marker)].color.g=g;
    wholePoint[wholePoint.indexOf(marker)].color.b=b;
}

int MessageServer::getid(QString username)
{
    auto infos=clients.values();
    QStringList usernames;
    for(auto info:infos)
        if(info.username==username) return info.userid;
//    return usernames.join(';');
}



























