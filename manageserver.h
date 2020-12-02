#ifndef MANAGESERVER_H
#define MANAGESERVER_H

#include <QTcpServer>
#include <QMap>

class ManageServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit ManageServer(QObject *parent=nullptr);
private:
    void incomingConnection(qintptr handle) override;
public slots:

};

#endif // MANAGESERVER_H
