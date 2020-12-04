#ifndef MESSAGESERVER_H
#define MESSAGESERVER_H
#include <QTcpServer>
#include <neuron_editing/v_neuronswc.h>
#include <QMutex>
#include "neuron_editing/neuron_format_converter.h"
#include "messagesocket.h"
class MessageServer : public QTcpServer
{
    struct UserInfo{
        QString username;
        int userid;
        QString::size_type sendedsize;
    };

    Q_OBJECT
public:
    static MessageServer* makeMessageServer(QString neuron);
    explicit MessageServer(QString neuron,QObject *parent = nullptr);
private:
    void incomingConnection(qintptr handle) override;
    QMap<QStringList,qint64> save(bool autosave=0);//f autosave?
    QString getUserList();

    //**********************************************

    void drawline(QString);
    void delline(QString);
    void addmarker(QString);
    void delmarekr(QString);
    void retypeline(QString);
    void retypemarker(QString);

public:
    QString port;
    QString neuron;

signals:
    void sendToAll(const QString &msg);
    void sendfiles(MessageSocket* socket,QStringList filepath);
    void sendmsgs(MessageSocket* socket,QStringList msglist);
    void messagecome();
public slots:
    void processmessage();
    QMap<QStringList,qint64> autosave();//
    void pushMessagelist(QString,bool);
    void userLogin(QString);
private:
    QTimer *timer;
    //*****************放在socket中作为静态变量
    QMap<MessageSocket *,UserInfo> clients;//(socket ,(user , msgcntsend))
    //*****************
    quint32 savedMessageIndex=0;
    QMap<QString,int> idmap;

    QStringList messagelist;

    QList <CellAPO> wholePoint;
    V3DLONG pointIndex;
    V_NeuronSWC_list segments;


};

namespace Map {

};
#endif // MESSAGESERVER_H
