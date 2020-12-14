#include "basicdatamanage.h"
#include <QMutex>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QVariant>

namespace DB {
    int count =0;
    QMutex locker;
    const QString databaseName="BrainTell";
    const QString dbHostName="localhost";
    const QString dbUserName="root";
    const QString dbPassword="1234";

    const QString TableForImage="TableForImage";//图像数据表
    const QString TableForPreReConstruct="TableForPreReConstruct";//预重建数据表
    const QString TableForFullSwc="TableForFullSwc";//重建完成数据表
    const QString TableForProof="TableForProof";//校验数据表
    const QString TableForCheckResult="TableForCheckResult";//校验结果数据表
    const QString TableForUser="TableForUser";

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
    //wait to finish
    int getid(QString username)
    {
        return username.toInt();
//        bool inserted=false;
//        QSqlDatabase db=getNewDbConnection();
//        if(!db.open()){
//            qFatal("cannot connect DB when processBrain");
//            throw "";
//        }
//        QSqlQuery query(db);
//        QString sql;
//    LABEL:
//        sql=QString("SELECT * INTO %1 where userName = ?").arg(TableForUser);
//        query.prepare(sql);
//        query.addBindValue(username);
//        if(!query.exec())
//        {
//            return -1;
//        }else{
//            if(query.next())
//            {
//                return query.value(0).toUInt();
//            }else if(!inserted)
//            {
//                sql=QString("INSERT INTO %1 userName = ?").arg(TableForUser);
//                query.addBindValue(username);
//                if(!query.exec())
//                {
//                    return -1;
//                }else
//                {
//                    inserted=true;
//                    goto LABEL;
//                }
//            }
//        }
//        return -1;
    }

//    bool addArborToDB(QString swcpath,QString swcname,QString position)
//    {
//        QSqlDatabase db=getNewDbConnection();
//        if(!db.open()){
//            qFatal("cannot connect DB when processBrain");
//            throw "";
//        }
//        QSqlQuery query(db);
//        QString sql=QString("INSERT IGNORE INTO %1 (Name,Neuron_id,Brain_id,Arbor_Position,Tag,Swc) VALUES (?,?,?,?,?,?)"
//                            ).arg(TableForProof);
//        query.prepare(sql);
//        query.addBindValue(swcname.left(swcname.size()-4));
//        query.addBindValue(swcname.left(swcname.lastIndexOf('_')));
//        query.addBindValue(swcname.left(swcname.indexOf('_')));
//        query.addBindValue(position);
//        query.addBindValue("0");
//        query.addBindValue(swcpath);
//        if(!query.exec()){

//            return false;
//        }

//        return true;
//    }

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
            QStringList filenames=dataDir.entryList();
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
            return {neuron+".ano",neuron+".ano.apo",neuron+".ano.eswc"};
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


}
