#ifndef MESSAGESERVER_H
#define MESSAGESERVER_H
#include <QTcpServer>
#include <neuron_editing/v_neuronswc.h>
#include <QMutex>
#include "neuron_editing/neuron_format_converter.h"
#include "messagesocket.h"
#include <QThread>
#include "basicdatamanage.h"
class MessageServer : public QTcpServer
{
    struct UserInfo{
        QString username;
        QString id;
        int userid;
        QString::size_type sendedsize;
    };

    Q_OBJECT
public:
    /**
     * @brief makeMessageServer
     * @param neuron neuronid
     * 在NeuronMapMessageServer中寻找对应neuron的Messageserver*，
     * 找到则返回Messageserver*，
     * 找不到则新建一个Messageserver，返回新建的Messageserver*
     * @return
     */
    static MessageServer* makeMessageServer(QString neuron);
    /**
     * @brief MessageServer
     * @param neuron neuronid
     * @param port 端口号
     * @param parent
     */
    explicit MessageServer(QString neuron,QString port,QThread *p,QObject *parent = nullptr);
    ~MessageServer();

public slots:
    /**
     * @brief userLogin
     * @param username
     * 用户登录，自动保存协作的文件，将自动保存的文件发送给用户，同时维护用的client
     */
    void userLogin(QString username);
    /**
     * @brief pushMessagelist
     * @param smg
     * 将收到的消息押入消息队列
     * 转发消息，一次最多转发5条
     */
    void pushMessagelist(QString msg);
    /**
     * @brief processmessage
     * 处理消息，一次最多处理5条
     */
    void processmessage();
    /**
     * @brief autosave
     * 自动保存
     * @return 返回保存文件的路径名和保存时的savedMessageIndex
     */
    QMap<QStringList,qint64> autosave();//

    void getBBSWC(QString paraStr)
    {
        auto swc_name_path=IP::getSwcInBlock(paraStr,segments);
        auto apo_name_path=IP::getApoInBlock(paraStr,wholePoint);
        emit sendfiles((MessageSocket*)(sender()),{swc_name_path.at(1),apo_name_path.at(1)});
        clients[(MessageSocket*)(sender())].sendedsize=savedMessageIndex;
    }
    void onstarted()
    {

    }
private:
    /**
     * @brief incomingConnection
     * @param handle
     * 为新的连接建立socjet
     */
    void incomingConnection(qintptr handle) override;
    /**
     * @brief save
     * @param autosave 控制是否是自动保存
     * @return 文件路径和保存时的savedMessageIndex
     */
    QMap<QStringList,qint64> save(bool autosave=0);//f autosave?
    /**
     * @brief getUserList
     * @return 当前所有用户的名称
     */
    QStringList getUserList();
    /**
     * @brief getid
     * @param username
     * @return 返回指定用户的id
     */
    int getid(QString username);
    //**********************************************

    void drawline(QString);
    void delline(QString);
    void addmarker(QString);
    void delmarekr(QString);
    void retypeline(QString);
    void retypemarker(QString);

    NeuronTree convertMsg2NT(QStringList &listwithheader,QString username=0,int from=0);

    inline double distance(const CellAPO &m1,const CellAPO &m2)const
    {
        return sqrt(
                    (m1.x-m2.x)*(m1.x-m2.x)+
                    (m1.y-m2.y)*(m1.y-m2.y)+
                    (m1.z-m2.z)*(m1.z-m2.z)
                );
    }

    vector<V_NeuronSWC>::iterator findseg(vector<V_NeuronSWC>::iterator begin,vector<V_NeuronSWC>::iterator end,const V_NeuronSWC seg);



signals:
    /**
     * @brief sendToAll 发送全局消息，所有在同一个MessageServer的client都可以收到
     * @param msg
     */
    void sendToAll(const QString &msg);
    /**
     * @brief sendfiles
     * @param socket
     * @param filepath
     * 给指定的socket发送文件
     */
    void sendfiles(MessageSocket* socket,QStringList filepath);
    /**
     * @brief sendmsgs
     * @param socket
     * @param msglist
     * 给指定的socket发送消息
     */
    void sendmsgs(MessageSocket* socket,QStringList msglist);
    /**
     * @brief messagecome
     * call function to process coming msg
     */
    void messagecome();

    void disconnectName(MessageSocket* socket);
public:
    QString port;
    QThread *p_thread=nullptr;

private:
    QString neuron;
    QTimer *timer;
    QMap<MessageSocket *,UserInfo> clients;//(socket ,(user , msgcntsend))
    QStringList messagelist;
    QList <CellAPO> wholePoint;
    V_NeuronSWC_list segments;
    quint32 savedMessageIndex;
    QFile msgLog;
    QTextStream msglogstream;
    const int defaulttype=3;
    const float ths=1;
//    enum ClientType {}
    static const QStringList clienttypes;

};
namespace Map {

};

//inline void print(V_NeuronSWC seg)
//{
//    qDebug()<<"----------------------------";
//    for(int i=0;i<seg.row.size();i++)
//    {
//        qDebug()<<i<<":"<<seg.row[i].x<<","<<seg.row[i].y<<" "<<seg.row[i].z;
//    }
//    qDebug()<<"-----------------------------";
//    return;
//}
#endif // MESSAGESERVER_H
