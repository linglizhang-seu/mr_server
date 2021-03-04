#include "basicdatamanage.h"
#include <QMutex>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QVariant>
#include "basic_c_fun/basic_surf_objs.h"
#include "neuron_editing/v_neuronswc.h"
#include "neuron_editing/neuron_format_converter.h"
#include <QNetworkAccessManager>
#include <QDateTime>
#include <QNetworkReply>
extern QString vaa3dPath;
extern QMap<QString,QStringList> m_MapImageIdWithRes;
extern QMap<QString,QString> m_MapImageIdWithDir;
namespace DB {
    uint count =0;
    QMutex locker;
    const QString databaseName="BrainTell";
    const QString dbHostName="localhost";
    const QString dbUserName="root";
    const QString dbPassword="1234";

    const QString TableForUser="TableForUser";
    const QString TableForUserScore="TableForUserScore";
    //    const QString TableForImage="TableForImage";//图像数据表
    //    const QString TableForPreReConstruct="TableForPreReConstruct";//预重建数据表
    //    const QString TableForFullSwc="TableForFullSwc";//重建完成数据表
    //    const QString TableForProof="TableForProof";//校验数据表
    //    const QString TableForCheckResult="TableForCheckResult";//校验结果数据表
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

    bool createTableForUser()
    {
        auto db=getNewDbConnection();
        if(!db.open())
        {
            qDebug()<<"Error:can not connect SQL";
            return false;
        }

        QSqlQuery query(db);
        {
            QString order="id INTEGER PRIMARY KEY AUTO_INCREMENT,"
                "userName VARCHAR(100) NOT NULL,"
                "passWord VARCHAR(20) NOT NULL,"
                "email VARCHAR(100) NOT NULL,"
                "nickName VARCHAR(50) NOT NULL"
                ;
            QString sql=QString("CREATE TABLE IF NOT EXISTS %1 (%2)").arg(TableForUser).arg(order);
            //        qDebug()<<sql;
            if(!query.exec(sql)){

                qDebug()<<query.lastError().text();
                return false;
            }
        }

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


        return true;
    }

    /**
     * @brief userLogin
     * @param userName
     * @param passward
     * @return 0:success;-1:db error;-2:no name;-3 wrong password
     */
    char userLogin(QStringList loginInfo,QStringList & res)
    {
        //
        auto db=getNewDbConnection();
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
     * @return 0:success;-1:db error;-2:same name
     */
    char userRegister(/*QString userName,QString passward*/const QStringList registerInfo)
    {
        //username,email,nickname,password,invite
        auto db=getNewDbConnection();
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
//                            {
//                                QNetworkAccessManager *accessManager = new QNetworkAccessManager;
//                                QNetworkRequest request;
//                                request.setUrl(QUrl("https://api.netease.im/nimserver/user/create.action"));
//                                request.setRawHeader("AppKey","0fda06baee636802cb441b62e6f65549");
//                                request.setRawHeader("Nonce","12345");
//                                QDateTime::currentSecsSinceEpoch();
//                                QString curTime=QString::number(QDateTime::currentSecsSinceEpoch());
////                                request.setRawHeader("CurTime",curTime);
////                                request.setRawHeader("CheckSum",);
//                                request.setRawHeader("Content-Type","application/x-www-form-urlencoded;charset=utf-8");
//                                QByteArray postData;
//                                postData.append(QString("accid=%1&password=%2").arg(registerInfo[0]).arg(registerInfo[3]));
//                                QNetworkReply* reply = accessManager->post(request, postData);

//                                QObject::connect(accessManager,&QNetworkAccessManager::finished,[=]{
//                                    if(reply->error()==QNetworkReply::NoError)
//                                    {
//                                        QByteArray bytes = reply->readAll();      //读取所有字节；
//                                        qDebug()<<"------------\n"<<bytes<<endl;
//                                        QJsonParseError error;
//                                        QJsonDocument doucment = QJsonDocument::fromJson(bytes, &error);
//                                        if (doucment.isObject())
//                                        {
//                                            QJsonObject obj = doucment.object();
//                                            QJsonValue val;
//                                            QJsonValue data_value;

//                                            if (obj.contains("code")) {
//                                                QString succ_msg = obj.value("code").toString();
//                                                qDebug()<<"code="<<succ_msg<<endl;
//                                            }

//                                            if (obj.contains("info")) {
//                                                QString succ_msg = obj.value("info").toString();
//                                                //ui->plainTextEdit->appendPlainText(tr("\n")+succ_msg);
//                                                qDebug() << succ_msg;
//                                            }

//                                        }
//                                    }
//                                });
//                            }
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

    char findPassword(QString data,QStringList & res)
    {
        auto db=getNewDbConnection();
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

    int getid(QString userName)
    {
        auto db=getNewDbConnection();
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

    int getScore(QString userName)
    {
        auto db=getNewDbConnection();
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
        }
        return 0;
    }

    bool setScore(QStringList userNames,std::vector<int> scores)
    {
        auto db=getNewDbConnection();
        if(!db.open())
        {
            qDebug()<<"Error:can not connect SQL";
            return false;
        }

//        UPDATE mytable SET
//            myfield = CASE id
//                WHEN 1 THEN 'value'
//                WHEN 2 THEN 'value'
//                WHEN 3 THEN 'value'
//            END
//        WHERE id IN (1,2,3)


        QSqlQuery query(db);
//        QString caseStr;
//        for(int i=0;i<scores.size();i++)
//        {
//            caseStr+=QString("WHEN %1 THEN %2\n").arg(userNames[i]).arg(scores[i]);
//        }

        QSqlQuery q;
        q.prepare("insert into myTable values (?, ?)");

        QString order=QString("update %1 set score = ? WHERE userName = ?").arg(TableForUserScore);
        qDebug()<<order;
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
           return true;
        }
        return false;
    }
}

namespace FE {
    QMap<QStringList,QStringList> getFilesPathFormFileName(QString msg)
    {
        QStringList filenames=msg.split(";",QString::SkipEmptyParts);
        QStringList filepaths;
        if(!QDir(QCoreApplication::applicationDirPath()+"/data").exists())
        {
            QDir(QCoreApplication::applicationDirPath()).mkdir("data");
        }
        for(auto filename:filenames)
        {
            filepaths.push_back(QCoreApplication::applicationDirPath()+"/data/"+filename);
        }
        return  {{filepaths,filenames}};
    }

    QStringList getFileNames(QString dirname)
    {
        QDir dataDir(QCoreApplication::applicationDirPath()+"/"+dirname);
        if(dataDir.exists())
        {
            dataDir.setSorting(QDir::Name);
            QStringList filenames=dataDir.entryList(QDir::Files);
            return filenames;
        }
        else{
            return {};
        }
    }

    bool processFileFromClient(QStringList filepaths)
    {
        QStringList filenames;
        for(auto filepath:filepaths)
            filenames.push_back(filepath.section('/',-1));
        if(!QDir(QCoreApplication::applicationDirPath()+"/data").exists())
        {
            QDir(QCoreApplication::applicationDirPath()).mkdir("data");
        }
        for(int i=0;i<filenames.size();i++)
        {
            QFile f(filepaths[i]);
            if(!f.rename(filepaths[i],QCoreApplication::applicationDirPath()+"/data/"+filenames[i]))
            {
                    qDebug()<<QString("Failed to move %1 to %2").arg(filepaths[i]).arg(QCoreApplication::applicationDirPath()+"/data/"+filenames[i])<<f.errorString();
                    return false;
            }
        }
        return true;
    }

    QStringList getLoadFile(QString neuron)
    {
        if(!QFile(QCoreApplication::applicationDirPath()+"/data/"+neuron).exists())
        {
            qDebug()<<"can not find "<<neuron;
        }else if(!QFile(QCoreApplication::applicationDirPath()+"/data/"+neuron+".apo").exists())
        {
            qDebug()<<"can not find "<<neuron+".apo";
        }if(!QFile(QCoreApplication::applicationDirPath()+"/data/"+neuron+".eswc").exists())
        {
            qDebug()<<"can not find "<<neuron+".eswc";
        }else
        {
            return {QCoreApplication::applicationDirPath()+"/data"+neuron,
                       QCoreApplication::applicationDirPath()+"/data"+ neuron+".apo",
                        QCoreApplication::applicationDirPath()+"/data"+neuron+".eswc"};
        }
        return {};
    }


    QStringList writeArborAno(QString name,QString pos,QString swc,QString dir)
    {
        //ano
        QString anoName=name+".ano";
        QString apoName=name+".ano.apo";
        QString swcName=name+".ano.eswc";

        auto NT=readSWC_file(swc);
        QStringList posList=pos.split('_');
        if(posList.size()!=3) return {};
        QList<CellAPO> cells;
        CellAPO cell;
        cell.x=posList[0].toDouble();cell.y=posList[1].toDouble();cell.z=posList[2].toDouble();
        cells.push_back(cell);

        QFile anofile(dir+"/"+anoName);
        anofile.open(QIODevice::WriteOnly);
        QString str1="APOFILE="+apoName;
        QString str2="SWCFILE="+swcName;
        QTextStream out(&anofile);
        out<<str1<<endl<<str2;
        anofile.close();

        writeESWC_file(dir+"/"+swcName,NT);
        writeAPO_file(dir+"/"+apoName,cells);

        return {dir+"/"+anoName,dir+"/"+apoName,dir+"/"+swcName};
    }
    QString cac_position(QString path)
    {
        QString result;
        if(path.contains("swc"))
        {
            result="";
        }
        else if(path.contains("apo"))
        {
            result="";
        }
        return result;
    }

    QString getFileList(QString conPath)
    {
        QString absPath=QCoreApplication::applicationDirPath()+"/data"+conPath;
        QFileInfoList fileInfos=QDir(absPath).entryInfoList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot,QDir::DirsFirst);
        QStringList res;
        for(auto info:fileInfos)
        {
            res.push_back(info.fileName()+" "+QString::number(info.isDir()?0:1));

        }
        return res.join(";;");
    }


}

namespace IP {
    void dirCheck(QString dirBaseName)
    {
        if(!QDir(QCoreApplication::applicationDirPath()+"/"+dirBaseName).exists())
        {
            QDir(QCoreApplication::applicationDirPath()).mkdir(dirBaseName);
        }
    }

    inline QProcess* getProcess()
    {
        return new QProcess;
    }

    inline void releaseProcess(QProcess *p)
    {
        delete p;
    }

    QString getImagePath(QString imageId,int res)
    {
        try {
            return QCoreApplication::applicationDirPath()
                    +"/image/"+m_MapImageIdWithDir.value(imageId)+"/"+m_MapImageIdWithRes.value(imageId).at(res-1);
        }  catch (...) {
            return "";
        }
    }

    int getImageRes(QString imageId)
    {
        return m_MapImageIdWithRes.value(imageId).size();
    }
    QStringList getImageBlock(QString msg)
    {
        qDebug()<<msg;
        /*
         * p1:brain_id;res;x;y;z;size;socket.descriptor
         * p2:Neuron_id/name
         * 返回：文件名，文件路径
         */
        QStringList paraList=msg.split(";",QString::SkipEmptyParts);
        QString brain_id=paraList.at(0).trimmed();//1. tf name/RES  2. .v3draw// test:17302;RES;x;y;z;b
        //0: 18465/1
        //1: 12520
        //2: 7000
        //3: 2916
        int res=paraList.at(1).toInt();//>0:1最大分辨率
        int xpos=paraList.at(2).toInt();//当前分辨率坐标
        int ypos=paraList.at(3).toInt();
        int zpos=paraList.at(4).toInt();
        int blocksize=paraList.at(5).toInt();

        dirCheck("tmp");
        QString apoName;
        {
            apoName=QCoreApplication::applicationDirPath()+"/tmp/"+paraList[6]+"__"+brain_id+"__"
                          + QString::number(xpos)+ "__"
                          + QString::number(ypos)+ "__"
                          + QString::number(zpos)+ "__"
                          + QString::number(blocksize)+"__"
                          + QString::number(blocksize)+ "__"
                          + QString::number(blocksize);
            CellAPO centerAPO;
            centerAPO.x=xpos;centerAPO.y=ypos;centerAPO.z=zpos;
            QList <CellAPO> List_APO_Write;
            List_APO_Write.push_back(centerAPO);
            if(!writeAPO_file(apoName+".apo",List_APO_Write))
            {
                qDebug()<<"fail to write apo";
                return {};//get .apo to get .v3draw
            }
        }
        QString namepart1=paraList[6]+"_"+brain_id+"_"+QString::number(blocksize)+"_";
        QString filepath;
        {
            filepath=getImagePath(brain_id,res);
            if(filepath.isEmpty()||!QFile(filepath).exists())  return {};
        }

        QString order =QString("xvfb-run -d %0/vaa3d -x %0/plugins/image_geometry/crop3d_image_series/libcropped3DImageSeries.so "
                                "-f cropTerafly -i %1/ %2.apo %3/tmp/%4 -p %5 %5 %5").arg(vaa3dPath)
                .arg(filepath).arg(apoName).arg(QCoreApplication::applicationDirPath()).arg(namepart1).arg(blocksize);
        qDebug()<<"order="<<order;
        auto p=getProcess();
        if(p->execute(order.toStdString().c_str())!=-1||p->execute(order.toStdString().c_str())>=0)
        {
            QFile f1(apoName+".apo"); qDebug()<<f1.remove();
            QString fName=namepart1+QString("%1.000_%2.000_%3.000.v3dpbd").arg(xpos).arg(ypos).arg(zpos);
            qDebug()<<fName<<"*************";releaseProcess(p);
            return {fName,QCoreApplication::applicationDirPath()+"/tmp/"+fName};
        }else
        {
            releaseProcess(p);
            return {};
        }
    }

    QStringList getSwcInBlock(const QString msg,const V_NeuronSWC_list& testVNL)
    {
        /*
         * p1:brain_id;res;x;y;z;size;socket.descriptor
         * p2:Neuron_id/name
         * 返回：文件名，文件路径
         */
        QStringList paraList=msg.split(";",QString::SkipEmptyParts);
        QString brain_id=paraList.at(0).trimmed();//1. tf name/RES  2. .v3draw// test:17302;RES;x;y;z;b
        //0: 18465/RESx18000x13000x5150
        //1: 12520
        //2: 7000
        //3: 2916
        int res=paraList.at(1).toInt();//>0
        int xpos=paraList.at(2).toInt();
        int ypos=paraList.at(3).toInt();
        int zpos=paraList.at(4).toInt();
        int blocksize=paraList.at(5).toInt();

        {
            QString name=brain_id;
            int x1=xpos-blocksize;
            int x2=xpos+blocksize;
            int y1=ypos-blocksize;
            int y2=ypos+blocksize;
            int z1=zpos-blocksize;
            int z2=zpos+blocksize;
            int cnt=pow(2,res-1);

            dirCheck("tmp");
            QString BBSWCNAME=QCoreApplication::applicationDirPath()+"/tmp/blockGet__"+name+QString("__%1__%2__%3__%4__%5__%6__%7.ano.eswc")
                    .arg(x1).arg(x2).arg(y1).arg(y2).arg(z1).arg(z2).arg(cnt);
            x1*=cnt;x2*=cnt;y1*=cnt;y2*=cnt;z1*=cnt;z2*=cnt;
            V_NeuronSWC_list tosave;
            for(int i=0;i<testVNL.seg.size();i++)
            {
                NeuronTree SS;
                V_NeuronSWC seg_temp =  testVNL.seg.at(i);
                seg_temp.reverse();
                for(int j=0;j<seg_temp.row.size();j++)
                {
                    if(seg_temp.row.at(j).x>=x1&&seg_temp.row.at(j).x<=x2
                            &&seg_temp.row.at(j).y>=y1&&seg_temp.row.at(j).y<=y2
                            &&seg_temp.row.at(j).z>=z1&&seg_temp.row.at(j).z<=z2)
                    {
                        tosave.seg.push_back(seg_temp);
                        break;
                    }
                }
            }
            qDebug()<<"get nt size:"<<tosave.seg.size();
            auto nt=V_NeuronSWC_list__2__NeuronTree(tosave);
            writeESWC_file(BBSWCNAME,nt);
            return {BBSWCNAME.right(BBSWCNAME.size()-BBSWCNAME.lastIndexOf('/')),BBSWCNAME};
        }
    }

    QStringList getApoInBlock(const QString msg,const QList <CellAPO>& wholePoint)
    {
        /*
         * p1:brain_id;res;x;y;z;size;socket.descriptor
         * p2:Neuron_id/name
         * 返回：文件名，文件路径
         */
        QStringList paraList=msg.split(";",QString::SkipEmptyParts);
        QString brain_id=paraList.at(0).trimmed();//1. tf name/RES  2. .v3draw// test:17302;RES;x;y;z;b
        //0: 18465/RESx18000x13000x5150
        //1: 12520
        //2: 7000
        //3: 2916
        int res=paraList.at(1).toInt();//>0
        int xpos=paraList.at(2).toInt();
        int ypos=paraList.at(3).toInt();
        int zpos=paraList.at(4).toInt();
        int blocksize=paraList.at(5).toInt();

        {
            QString name=brain_id;
            int x1=xpos-blocksize;
            int x2=xpos+blocksize;
            int y1=ypos-blocksize;
            int y2=ypos+blocksize;
            int z1=zpos-blocksize;
            int z2=zpos+blocksize;
            int cnt=pow(2,res-1);


            dirCheck("tmp");
            QString BBAPONAME=QCoreApplication::applicationDirPath()+"/tmp/blockGet__"+name+QString("__%1__%2__%3__%4__%5__%6__%7.ano.apo")
                    .arg(x1).arg(x2).arg(y1).arg(y2).arg(z1).arg(z2).arg(cnt);
            x1*=cnt;x2*=cnt;y1*=cnt;y2*=cnt;z1*=cnt;z2*=cnt;
            qDebug()<<"x1,x2,y1,y2,z1,z2"<<x1<<x2<<y1<<y2<<z1<<z2;
            QList <CellAPO> to_save;
            for(auto marker:wholePoint)
            {
                if(marker.x>=x1&&marker.x<=x2
                  &&marker.y>=y1&&marker.y<=y2
                  &&marker.z>=z1&&marker.z<=z2)
                {
                    to_save.append(marker);
                }
            }
            qDebug()<<"to_save.size()="<<to_save.size();
            writeAPO_file(BBAPONAME,to_save);
            return {BBAPONAME.right(BBAPONAME.size()-BBAPONAME.lastIndexOf('/')),BBAPONAME};
        }
    }
}
