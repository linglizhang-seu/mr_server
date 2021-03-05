#include "basicdatamanage.h"
#include <QMutex>

#include "basic_c_fun/basic_surf_objs.h"
#include "neuron_editing/v_neuronswc.h"
#include "neuron_editing/neuron_format_converter.h"



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

