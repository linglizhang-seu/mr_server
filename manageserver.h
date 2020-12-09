#ifndef MANAGESERVER_H
#define MANAGESERVER_H

#include <QTcpServer>
#include <QMap>
/**
 * @brief The ManageServer class
 * ManageServer 程序启动后自动启动，等待客户端的连接
 * 继承自QTcpServer
 */
class ManageServer : public QTcpServer
{
    Q_OBJECT
public:
    /**
     * @brief ManageServer的构造函数
     * @param parent：QObject* 使得ManageServer是QObject的子类
     * 这是一个默认构造函数
     */
    explicit ManageServer(QObject *parent=nullptr);
private:
    /**
     * @brief incomingConnection
     * @param handle:socket连接的描述符
     * 该函数重写自QTcpServer,当有新连接来是创建一个ManageSocket，并设置其连接描述符
     */
    void incomingConnection(qintptr handle) override;
public slots:

};

#endif // MANAGESERVER_H

/*
 * Messagaer
 * -------------------------
 * + ManagerServer()
 * - incommingConnection(qintptr)
 */
