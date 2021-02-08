#include "basicdatamanage.h"
#include <QMutex>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QVariant>

namespace DB {
    uint count =0;
    QMutex locker;
    const QString databaseName="BrainTell";
    const QString dbHostName="localhost";
    const QString dbUserName="root";
    const QString dbPassword="1234";

    const QString TableForUser="TableForUser";
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
        QString order="id INTEGER PRIMARY KEY AUTO_INCREMENT,"
                "userId VARCHAR(100) NOT NULL,"
                "PassWord VARCHAR(20) NOT NULL,"
                "userName VARCHAR(50) NOT NULL"
                ;
        QString sql=QString("CREATE TABLE IF NOT EXISTS %1 (%2)").arg(TableForUser).arg(order);
        if(!query.exec(sql)){
            qDebug()<<query.lastError().text();
            return false;
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
        auto db=getNewDbConnection();
        if(!db.open())
        {
            qDebug()<<"Error:can not connect SQL";
            return -1;
        }
        QSqlQuery query(db);
        QString order=QString("SELECT * FROM %1 WHERE userId = ?").arg(TableForUser);
        query.prepare(order);
        query.addBindValue(loginInfo[0]);
        if(query.exec()){
            if(query.next())
            {
                return -2;
            }else
            {
                if(query.value(2).toString()==loginInfo[1])
                {
                    res=loginInfo;
                    res.push_back(query.value(3).toString());
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
        auto db=getNewDbConnection();
        if(!db.open())
        {
            qDebug()<<"Error:can not connect SQL";
            return -1;
        }
        QSqlQuery query(db);
        QString order=QString("SELECT * FROM %1 WHERE userId = ?").arg(TableForUser);
        query.prepare(order);
        query.addBindValue(registerInfo[0]);
        if(query.exec()){
            if(query.next())
            {
                return -2;
            }else
            {
                order=QString("INSERT INTO %1 (userId,PassWord,userName) VALUES (?,?)").arg(TableForUser);
                query.prepare(order);
                query.addBindValue(registerInfo[0]);
                query.addBindValue(registerInfo[1]);
                query.addBindValue(registerInfo[2]);
                if(query.exec()) return 0;
                else return -1;
            }
        }else
        {
            return -1;
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
        QString order=QString("SELECT * FROM %1 WHERE userId = ?").arg(TableForUser);
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
        QString order=QString("SELECT * FROM %1 WHERE Brainid = ?").arg(TableForUser);
        query.prepare(order);
        query.addBindValue(userName);
        if(query.exec()){
            if(query.next())
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
        QDir dataDir=QDir(QCoreApplication::applicationDirPath()+"/data");
        if(!dataDir.exists(neuron+".ano"))
        {
            qDebug()<<"can not find "<<neuron+".ano";
        }else if(!dataDir.exists(neuron+".ano.apo")){
            qDebug()<<"can not find "<<neuron+".ano.apo";
        }else if(!dataDir.exists(neuron+".ano.eswc")){
            qDebug()<<"can not find "<<neuron+".ano.eswc";
        }else
        {
            return {QCoreApplication::applicationDirPath()+"/data/"+neuron+".ano",
                       QCoreApplication::applicationDirPath()+"/data/"+ neuron+".ano.apo",
                        QCoreApplication::applicationDirPath()+"/data/"+neuron+".ano.eswc"};
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
        QString absPath=QCoreApplication::applicationFilePath()+"/data"+conPath;
        QFileInfoList fileInfos=QDir(absPath).entryInfoList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot,QDir::DirsFirst);
        QStringList res;
        for(auto info:fileInfos)
        {
            res.push_back(info.fileName()+" "+QString::number(info.isDir()?0:1));

        }
        return res.join(";;");
    }


}
