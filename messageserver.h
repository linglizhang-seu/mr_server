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
    int getid(QString username);

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

    QStringList messagelist;
    QList <CellAPO> wholePoint;
    V3DLONG pointIndex;
    V_NeuronSWC_list segments;


};
/*
 *
 * MessageServer
 * -----------------------------
 * + port:QString
 * + neuronid:QString
 * - timer:QTimer
 * - clients:QMap<Messagesocket*,UserInfo> clients
 * - savedMessageIndex:quint32
 * - messagelist:QStringlist
 * - wholePoint:QList<CellApo>
 * - pointIndex:V3DLONG
 * - segments:V_NeuronSWC_list
 * ------------------------------------
 * slot
 * + processmessage()
 * + autosave()
 * + pushMessageList(QString,bool)
 * + userLogin(QString);
 * 静态函数 makeMessageServer(neurn_id)
 * - incomingConnection(qintptr)
 * - save(bool)
 * - getuserlist()
 * - drawline()
 * - delline()
 * - addmarker()
 * - delmarekr()
 * - retypeline()
 * - retypemarker()
 * - getid(QString)
 * signal
 * sendToAll(QString)
 * sendfiles(Messagesocket*,filepath)
 * sendmsgs(Messagesocket*,msgs)
 * messagecome()
*/
namespace Map {

};
#endif // MESSAGESERVER_H
