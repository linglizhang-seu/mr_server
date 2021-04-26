#ifndef DBFUNCTION_H
#define DBFUNCTION_H

#include <QSqlDatabase>
#include <QString>
#include <QStringList>
#include <QList>
#include "basic_c_fun/basic_surf_objs.h"
#include <QSqlQuery>
#include <neuron_editing/neuron_format_converter.h>
/**
 *@brief 数据库相关函数
 * 2020-12-07:只有获取数据库连接的代码
 */
namespace DB  {
/**
     * @brief getNewDbConnection
     * @return
     * 返回一个数据库的连接QSqlDatabase
     */
//    QSqlDatabase getNewDbConnection();
    bool initDB(QSqlDatabase&);

    char userLogin(QSqlDatabase&,QStringList loginInfo,QStringList & res);
    char userRegister(QSqlDatabase&,QStringList registerInfo);
    char findPassword(QSqlDatabase&,QString data,QStringList & res);
    int getid(QSqlDatabase&,QString userName);

    int getScore(QSqlDatabase&,QString id);
    bool setScores(QSqlDatabase&,QStringList userNames,std::vector<int> scores);
     bool registerCommunicate(const QStringList &registerInfo);
}



namespace FE {
    QMap<QStringList,QStringList> getFilesPathFormFileName(QString msg);

    QStringList getFileNames(QString dirname);

    bool processFileFromClient(QStringList filepaths);

    QStringList getLoadFile(QString neuron);

    QString getFileList(QString conPath);

    QStringList writeArborAno(QString name,QString pos,QString swc,QString dir);
    QString cac_position(QString path);


}

namespace IP {
    int getImageRes(QString imageId);
    QStringList getImageBlock(QString msg);
    QStringList getSwcInBlock(const QString msg,const V_NeuronSWC_list& testVNL);
    QStringList getApoInBlock(const QString msg,const QList <CellAPO>& wholePoint);
}






#endif // DBFUNCTION_H
