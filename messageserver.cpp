#include "messageserver.h"
#include "somefunction.h"
#include "ThreadPool.h"
#include "basic_c_fun/basic_surf_objs.h"
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
    timer=new QTimer;
    messagelist.clear();
    wholePoint.clear();
    segments.clear();

    wholePoint=readAPO_file(map.keys().at(0).at(1));
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
        if(clients.size()==0) {
            Map::NeuronMapMessageServer.remove(Map::NeuronMapMessageServer.key(this));
            this->deleteLater();
        }

        delete messagesocket;
        releaseThread(thread);
        emit sendToAll(getUserList());
    });
    connect(this,SIGNAL(sendfile(MessageSocket*,QStringList)),messagesocket,SLOT(sendfiles(MessageSocket*,QStringList)));
}



void MessageServer::userLogin(QString name)
{
    auto t=autosave();
    auto p=(MessageSocket*)(sender());
    emit sendfile(p,t.keys().at(0));
    clients.insert(p,{{name,t.values().at(0)}});
}

QMap<QStringList,qint64> MessageServer::autosave()
{
    return save(1);
}

QMap<QStringList,qint64> MessageServer::save(bool f)
{
    return {};
}
