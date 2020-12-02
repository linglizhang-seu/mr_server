#include "dbfunction.h"
#include <QMutex>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QVariant>
extern QString TableForImage;//图像数据表
extern QString TableForPreReConstruct;//预重建数据表
extern QString TableForFullSwc;//重建完成数据表
extern QString TableForProof;//校验数据表
extern QString TableForCheckResult;//校验结果数据表

namespace DB {
    int count =0;
    QMutex locker;
    const QString databaseName="BrainTell";
    const QString dbHostName="localhost";
    const QString dbUserName="root";
    const QString dbPassword="1234";

    QSqlDatabase getNewDbConnection()
    {
        locker.lock();
        QSqlDatabase db=QSqlDatabase::addDatabase("QMYSQL",QString::number(count++));
        locker.unlock();
        db.setDatabaseName(databaseName);
        db.setHostName(dbHostName);
        db.setUserName(dbUserName);
        db.setPassword(dbPassword);
        return db;
    }

    bool addArborToDB(QString swcpath,QString swcname,QString position)
    {
        QSqlDatabase db=getNewDbConnection();
        if(!db.open()){
            qFatal("cannot connect DB when processBrain");
            throw "";
        }
        QSqlQuery query(db);
        QString sql=QString("INSERT IGNORE INTO %1 (Name,Neuron_id,Brain_id,Arbor_Position,Tag,Swc) VALUES (?,?,?,?,?,?)"
                            ).arg(TableForProof);
        query.prepare(sql);
        query.addBindValue(swcname.left(swcname.size()-4));
        query.addBindValue(swcname.left(swcname.lastIndexOf('_')));
        query.addBindValue(swcname.left(swcname.indexOf('_')));
        query.addBindValue(position);
        query.addBindValue("0");
        query.addBindValue(swcpath);
        if(!query.exec()){

            return false;
        }

        return true;
    }

    bool addFullSwcToDB(QStringList filepaths,QStringList filenames)
    {
        return true;
    }
}
