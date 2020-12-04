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
        throw "";
    }
    timer=new QTimer();
    connect(timer,&QTimer::timeout,this,&MessageServer::autosave);
    timer->start(5*60*1000);
    messagelist.clear();
    wholePoint.clear();
    segments.clear();
    savedMessageIndex=0;
    clients.clear();
    idmap.clear();

    wholePoint=readAPO_file(map.keys().at(0).at(1));
    pointIndex=wholePoint.size();
    auto NT=readSWC_file(map.values().at(0).at(2));
    segments=NeuronTree__2__V_NeuronSWC_list(NT);
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
        emit sendToAll(getUserList());
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

void MessageServer::drawline(QString msg)
{

}
void MessageServer::delline(QString msg)
{

}
void MessageServer::addmarker(QString msg)
{

}
void MessageServer::delmarekr(QString msg)
{

}
void MessageServer::retypeline(QString msg)
{

}
void MessageServer::retypemarker(QString msg)
{

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




























