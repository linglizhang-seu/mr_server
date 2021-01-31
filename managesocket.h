#ifndef MANAGESOCKET_H
#define MANAGESOCKET_H

#include "tcpsocket.h"
#include "messageserver.h"
/**
 * @brief The ManageSocket class
 * 继承自QObject
 * 管理客户端的command请求：下载文件、加载文件和上传文件
 */

class ManageSocket:public TcpSocket
{
    Q_OBJECT
public:
    /**
     * @brief ManageSocket
     * @param handle socket的描述符
     * @param parent QT父类
     * 初始化类的各个属性，建立一个新的QTcpSocket，并设置其socket描述符
     * 建立该对象的信号和槽连接     *
     */
    ManageSocket(qintptr handle,QObject * parent=nullptr):TcpSocket(handle,parent)
    {
        connect(this,&TcpSocket::disconnected,this,[=]{
            deleteLater();
        });
    }
private:
    /**
     * @brief processMsg 处理command
     * @param msglist command队列
     * command格式：
     * 下载："(.*):DownloadANO" （.*）为文件名列表
     * 加载神经元："(.*):LoadANO")
     */
    bool processMsg( const QString msg);
    /**
     * @brief processFile 处理接受的文件队列
     * 冗余设计，可以及接受多个文件
     * @param filePaths 文件路径列表
     */
    bool processFile( const QString filePath);
    /**
     * @brief makeMessageServer 调用MessageServer的静态，创建协作的MessageServer
     * @param neuron 要协作的神经元编号
     * @return
     */
    MessageServer* makeMessageServer(QString neuron);
};
#endif // MANAGESOCKET_H

