#ifndef MESSAGESERVER_H
#define MESSAGESERVER_H
#include <QTcpServer>
#include <neuron_editing/v_neuronswc.h>
#include <QMutex>
#include "neuron_editing/neuron_format_converter.h"
#include "messagesocket.h"
class MessageServer : public QTcpServer
{
    Q_OBJECT
public:
    static MessageServer* makeMessageServer(QString neuron);
    explicit MessageServer(QString neuron,QObject *parent = nullptr);
private:
    void incomingConnection(qintptr handle) override;
    QMap<QStringList,qint64> save(bool f=0);//f autosave?
    QString getUserList();
public:
    QString port;
    QString neuron;

signals:
    void sendToAll(const QString &msg);
    void sendfile(MessageSocket* socket,QStringList filepath);

public slots:
    QMap<QStringList,qint64> autosave();//
    void userLogin(QString);

private:
    QTimer *timer;
    //*****************放在socket中作为静态变量
    QMap<MessageSocket *,QMap<QString,QString::size_type>> clients;
    quint32 savedMessageIndex=0;
    QReadWriteLock lock_savedMessageIndex;
    //*****************

    QStringList messagelist;
    QList <CellAPO> wholePoint;
    V_NeuronSWC_list segments;
};

namespace Map {

};
#endif // MESSAGESERVER_H
