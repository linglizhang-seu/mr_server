#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QVariant>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QDateTime>
#include <QNetworkReply>
#include "basicdatamanage.h"
#include "sha1.h"
#include <QEventLoop>
#include <QObject>
namespace DB {
    uint count =0;
    QMutex locker;
//    const QString databaseName="Hi5";
//    const QString dbHostName="localhost";
//    const QString dbUserName="hi5";
//    const QString dbPassword="!helloHi5";

    QString TableForUser="TableForUser";
    QString TableForUserScore="TableForUserScore";
    QString TableForSomaList="somaList";
    //    const QString TableForImage="TableForImage";//图像数据表
    //    const QString TableForPreReConstruct="TableForPreReConstruct";//预重建数据表
    //    const QString TableForFullSwc="TableForFullSwc";//重建完成数据表
    //    const QString TableForProof="TableForProof";//校验数据表
    //    const QString TableForCheckResult="TableForCheckResult";//校验结果数据表
//    QSqlDatabase getNewDbConnection()
//    {
//        locker.lock();
//        QSqlDatabase db=QSqlDatabase::addDatabase("QMYSQL",QString::number(count++));
//        locker.unlock();
//        db.setDatabaseName(databaseName);
//        db.setHostName(dbHostName);
//        db.setUserName(dbUserName);
//        db.setPassword(dbPassword);
//        return db;
//    }

    bool registerCommunicate(const QStringList &registerInfo)
    {


            QNetworkAccessManager *accessManager = new QNetworkAccessManager;
            QNetworkRequest request;
            request.setUrl(QUrl("https://api.netease.im/nimserver/user/create.action"));
            request.setRawHeader("AppKey","41f5e68ee7c226595dae175eb979061f");
            request.setRawHeader("Nonce","12345");
            QDateTime::currentSecsSinceEpoch();
            QString curTime=QString::number(QDateTime::currentSecsSinceEpoch());
            request.setRawHeader("CurTime",curTime.toStdString().c_str());
            QString appSecret = "c9266fa8edee";
            request.setRawHeader("CheckSum",sha1(QString(appSecret+"12345"+curTime).toStdString()).c_str());
            request.setRawHeader("Content-Type","application/x-www-form-urlencoded;charset=utf-8");
            QByteArray postData;
            postData.append(QString("accid=%1&name=%2&token=%3").arg(registerInfo[0]).arg(registerInfo[2]).arg(registerInfo[3]));

            QNetworkReply* reply = accessManager->post(request, postData);

            QEventLoop eventLoop;
            QObject::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
            eventLoop.exec(QEventLoop::ExcludeUserInputEvents);

            auto res=reply->error();
            if(res==QNetworkReply::NoError)
            {
                QByteArray bytes = reply->readAll();      //读取所有字节；
                qDebug()<<bytes;
                QJsonParseError error;
                QJsonDocument doucment = QJsonDocument::fromJson(bytes, &error);
                if (doucment.isObject())
                {
                    QJsonObject obj = doucment.object();
                    QJsonValue val;
                    QJsonValue data_value;

                    if (obj.contains("code")&&obj.value("code").toInt()==200)
                            return true;
                }
            }else
            {
                qDebug()<<res;
                return false;
            }

    }

    bool initDB(QSqlDatabase &db)
    {
//        auto db=getNewDbConnection();
        if(!db.open())
        {
            qDebug()<<"Error:can not connect SQL,"<<db.lastError().text();
            return false;
        }
        //TableForUser
        QSqlQuery query(db);
        {
            QString order="id INTEGER PRIMARY KEY AUTO_INCREMENT,"
                "userName VARCHAR(100) NOT NULL,"
                "passWord VARCHAR(20) NOT NULL,"
                "email VARCHAR(100) NOT NULL,"
                "nickName VARCHAR(50) NOT NULL"
                ;
            QString sql=QString("CREATE TABLE IF NOT EXISTS %1 (%2)").arg(TableForUser).arg(order);
//                    qDebug()<<sql;
            if(!query.exec(sql)){
                qDebug()<<query.lastError().text();
                return false;
            }
        }
        //TableForUserScore
        {
            QString order="userName VARCHAR(100) PRIMARY KEY NOT NULL,"
                "score int NOT NULL"
                ;
            QString sql=QString("CREATE TABLE IF NOT EXISTS %1 (%2)").arg(TableForUserScore).arg(order);
            if(!query.exec(sql)){

                qDebug()<<query.lastError().text();
                return false;
            }
        }

        {
            QString order="id varchar(100) primary key not null, x int not null, y int not null, z int not null,acktime Date";
            QString sql=QString("create table if not exists %1 (%2)").arg(TableForSomaList).arg(order);
            if(!query.exec(sql)){

                qDebug()<<query.lastError().text();
                return false;
            }
        }


        return true;
    }

    /**
     * @brief userLogin
     * @param userName
     * @param passward
     * @return 0:success;-1:db error;-2:no name;-3 wrong password
     */
    char userLogin(QSqlDatabase &db,QStringList loginInfo,QStringList & res)
    {
        if(!db.open())
        {
            qDebug()<<"Error:can not connect SQL";
            return -1;
        }
        QSqlQuery query(db);
        QString order=QString("SELECT * FROM %1 WHERE userName = ?").arg(TableForUser);
        query.prepare(order);
        query.addBindValue(loginInfo[0]);
        if(query.exec()){
            if(!query.next())
            {
                return -2;
            }else
            {
                if(query.value(2).toString()==loginInfo[1])
                {
                    res=loginInfo;
                    res.push_back(query.value(3).toString());
                    res.push_back(query.value(4).toString());
                    return 0;
                }
                else return -3;
            }
        }else
        {
            return -1;
        }
    }

    /**
     * @brief userRegister
     * @param userName
     * @param passward
     * @return 0:success;-1:db error;-2:same name;-3:netease error
     */
    char userRegister(QSqlDatabase &db,const QStringList registerInfo)
    {
        //username,email,nickname,password,invite
        if(!registerCommunicate(registerInfo))
            return -3;
        if(!db.open())
        {
            qDebug()<<"Error:can not connect SQL";
            return -1;
        }
        QSqlQuery query(db);
        {
            QString order=QString("SELECT * FROM %1 WHERE userName = ?").arg(TableForUser);
            query.prepare(order);
            query.addBindValue(registerInfo[0]);
            if(query.exec()){
                if(query.next())
                {
                    return -2;
                }else
                {
                    order=QString("INSERT INTO %1 (userName,passWord,email,nickName) VALUES (?,?,?,?)").arg(TableForUser);
                    query.prepare(order);
                    query.addBindValue(registerInfo[0]);
                    query.addBindValue(registerInfo[3]);
                    query.addBindValue(registerInfo[1]);
                    query.addBindValue(registerInfo[2]);
                    if(query.exec())
                    {
                        order=QString("INSERT INTO %1 (userName,score) VALUES(?,?)").arg(TableForUserScore);
                        query.prepare(order);
                        query.addBindValue(registerInfo[0]);
                        query.addBindValue(0);
                        if(query.exec())
                        {
                             return 0;
                        }
                    }
                    else return -1;
                }
            }else
            {
                return -1;
            }
        }

    }


    char findPassword(QSqlDatabase &db,QString data,QStringList & res)
    {
        if(!db.open())
        {
            qDebug()<<"Error:can not connect SQL";
            return -1;
        }
        QSqlQuery query(db);
        QString order=QString("SELECT * FROM %1 WHERE email = ?").arg(TableForUser);
        query.prepare(order);
        query.addBindValue(data);
        if(query.exec()){
            if(query.next())
            {
                return -2;
            }else
            {
                res.push_back(query.value(1).toString());
                res.push_back(query.value(2).toString());
//                res={query.value(1).toString(),query.value(2).toString()};
                    return 0;
            }
        }else
        {
            return -1;
        }
    }

    int getid(QSqlDatabase &db,QString userName)
    {
        if(!db.open())
        {
            qDebug()<<"Error:can not connect SQL";
            return -1;
        }
        QSqlQuery query(db);
        QString order=QString("SELECT id FROM %1 WHERE userName = ?").arg(TableForUser);
        query.prepare(order);
        query.addBindValue(userName);
        if(query.exec()){
            if(!query.next())
            {
                return -2;
            }else
            {
                return query.value(0).toUInt();
            }
        }else
        {
            return -1;
        }
    }

    int getScore(QSqlDatabase &db,QString userName)
    {
        if(userName.isEmpty()) return -2;
        if(!db.open())
        {
            qDebug()<<"Error:can not connect SQL";
            return -1;
        }
        QSqlQuery query(db);
        QString order=QString("SELECT score FROM %1 WHERE userName = ?").arg(TableForUserScore);
        query.prepare(order);
        query.addBindValue(userName);
        if(query.exec()&&query.next())
        {
           return query.value(0).toUInt();
        }else
        {
            order=QString("INSERT INTO %1 (userName,score) VALUES(?,?)").arg(TableForUserScore);
            query.prepare(order);
            query.addBindValue(userName);
            query.addBindValue(0);
            if(query.exec())
                return 0;
            else
                return -1;
        }

    }

    bool setScores(QSqlDatabase &db,QStringList userNames,std::vector<int> scores)
    {
        if(!db.open())
        {
            qDebug()<<"Error:can not connect SQL";
            return false;
        }

        QSqlQuery query(db);
        QString order=QString("update %1 set score = ? WHERE userName = ?").arg(TableForUserScore);
        query.prepare(order);
        QVariantList ints;
        for(auto v:scores)
        {
            ints<<v;
        }
        query.addBindValue(ints);
        query.addBindValue(userNames);

        if(query.execBatch())
        {
            qDebug()<<"set score success"<<endl;
           return true;
        }
       std::cerr<<"set score failed"<<endl;
        return false;
    }

    QString getFirstK(QSqlDatabase &db,int K)
    {
        QString res;
        if(!db.open())
        {
            qDebug()<<"Error:can not connect SQL";
            return res;
        }
        QSqlQuery query(db);
        QString order=QString("select * from %1 order by score desc limit 0,%2").arg(TableForUserScore).arg(K);
        query.prepare(order);
        if(query.exec())
            while (query.next()) {
                res+=query.value(0).toString()+';'+query.value(1).toString()+';';
            }
        return res;
    }

    void zipDayTask(QSqlDatabase &db,QString dirname)
    {
        if(!db.open())
        {
            qDebug()<<"Error:can not connect SQL";
        }
        QSqlQuery query(db);
        QString order=QString("select id,x,y,z from %1 where acktime = %2")
                .arg(TableForSomaList).
                arg(QDate::currentDate().addDays(-1).toString("yyyy-MM-DD"));

        query.prepare(order);
        if(query.exec())
        {
            QMap<QString,QVector<int>> somalist;
            while(query.next())
            {
                somalist[query.value(0).toString()]={query.value(1).toInt(),query.value(2).toInt(),query.value(3).toInt()};
            }
            {
                auto tempName=QCoreApplication::applicationDirPath()+"/tmp";
                QDir dir(tempName);
                auto foldname=QDate::currentDate().toString("yyyy-MM-DD");
                dir.mkdir(foldname);
                for(auto s:somalist.keys())
                {
                    QString brainId=s.split("_").at(0);
                    QFile::copy(QCoreApplication::applicationDirPath()+"/data/"+brainId+"/"+s+"/"+s+".ano",
                                tempName+"/"+foldname+"/"+s+".ano");
                    QFile::copy(QCoreApplication::applicationDirPath()+"/data/"+brainId+"/"+s+"/"+s+".ano.apo",
                                tempName+"/"+foldname+"/"+s+".ano.apo");

                    auto nt=readSWC_file(QCoreApplication::applicationDirPath()+"/data/"+brainId+"/"+s+"/"+s+".ano.eswc");
                    for(auto &node:nt.listNeuron)
                    {
                        node.x+=(somalist[s][0]-256);
                        node.y+=(somalist[s][1]-256);
                        node.z+=(somalist[s][2]-256);
                    }
                    writeESWC_file(tempName+"/"+foldname+"/"+s+".ano.eswc",nt);
                }
                system(QString("zip -q -r %1/%2.zip %3").arg(dirname).arg(foldname).arg(tempName+"/"+foldname).toStdString().c_str());
                dir.cd(foldname);dir.removeRecursively();
            }
        }

    }
    bool setAck(QSqlDatabase &db,QString id)
    {
        if(!db.open())
        {
            qDebug()<<"Error:can not connect SQL";
        }
        QSqlQuery query(db);
        QString order=QString("update %1 set acktime = NOW()").arg(TableForSomaList);
        if(!query.exec())
        {
            return false;
        }
        return true;
    }

    bool insertAck(QSqlDatabase &db,QString somalist)
    {
        if(!db.open())
        {
            qDebug()<<"Error:can not connect SQL";
        }
        QSqlQuery query(db);
        QVector<QString> ss;
        QVariantList xs,ys,zs;
        auto somas=QDir(somalist).entryInfoList(QDir::Files);
        for(auto s:somas)
        {
            auto apo=readAPO_file(s.absoluteFilePath());
            ss.push_back(s.baseName());
            xs<<int(apo.at(0).x);
            ys<<int(apo.at(0).y);
            zs<<int(apo.at(0).z);

            QDir dir(QCoreApplication::applicationDirPath()+"/data");
            dir.mkdir(s.baseName().split('_').at(0));
            dir.cd(s.baseName().split('_').at(0));
            dir.mkdir(s.baseName());
            QFile f(QCoreApplication::applicationDirPath()+"/data/"
                    +s.baseName().split('_').at(0)
                    +"/"+s.baseName()
                    +"/"+s.baseName()
                    +QString("_x%1.000_y%2.000_z%3.000.ano")
                    .arg(int(apo.at(0).x))
                    .arg(int(apo.at(0).y))
                    .arg(int(apo.at(0).z))
                    );
        }
        QString order = "INSERT INTO %1 (id,x,y,z) Values(?,?,?,?)";
        query.prepare(order);
        query.addBindValue(ss);
        query.addBindValue(xs);
        query.addBindValue(ys);
        query.addBindValue(zs);
        if(!query.execBatch())
            return false;
        return true;
    }

}
