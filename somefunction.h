#ifndef SOMEFUNCTION_H
#define SOMEFUNCTION_H
#include <QString>
#include <QStringList>
#include <QList>
#include "basic_c_fun/basic_surf_objs.h"
#include "dbfunction.h"
#include <QSqlQuery>
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

int getid(QString name)
{
    return 0;
}
QMap<QStringList,QStringList> getANOFILE(QString neuronid,bool& f)
{
    QSqlDatabase db=DB::getNewDbConnection();
    if(!db.open()){
        qFatal("cannot connect DB when processBrain");
        f=false;
        return {};
    }
    QSqlQuery query(db);

    auto list = neuronid.trimmed().split(";",QString::SkipEmptyParts);

    if(list.size()!=2)
    {
        f=false;return {};
    }
    if(list[0]=="FULL")
    {
        QString sql=QString("SELECT Ano,Apo,Swc From %1 WHERE Neuron_id = ?").arg("TableForFullSwc");
        query.prepare(sql);
        query.addBindValue(list[1]);
        if(query.exec()){
            if(query.next()){
                QStringList filepaths;
                QStringList filenames;
                {
                    filepaths.push_back(query.value(0).toString());
                    filenames.push_back(query.value(0).toString().section('/',-1));
                    filepaths.push_back(query.value(1).toString());
                    filenames.push_back(query.value(1).toString().section('/',-1));
                    filepaths.push_back(query.value(2).toString());
                    filenames.push_back(query.value(2).toString().section('/',-1));
                }
                f=true;
                return {{filepaths,filenames}};
            }
        }
    }else if(list[0]=="ARBOR")
    {
        QString sql=QString("SELECT Name,Arbor_position,Swc From %1 WHERE Name = ?").arg("TableForProof");
        query.prepare(sql);
        query.addBindValue(list[1]);
        if(query.exec()){
            if(query.next()){
                QStringList filepaths=writeArborAno(query.value(0).toString(),query.value(1).toString(),query.value(2).toString(),QCoreApplication::applicationDirPath()+"/tmp");
                QStringList filenames;
                filenames.push_back(filepaths[0].section('/',-1));
                filenames.push_back(filepaths[1].section('/',-1));
                filenames.push_back(filepaths[2].section('/',-1));
                f=true;
                return {{filepaths,filenames}};
            }
        }
    }else{
        f=false;
        return {};
    }
}
#endif // SOMEFUNCTION_H
