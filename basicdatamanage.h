#ifndef DBFUNCTION_H
#define DBFUNCTION_H

#include <QSqlDatabase>
#include <QString>
#include <QStringList>
#include <QList>
#include "basic_c_fun/basic_surf_objs.h"
#include <QSqlQuery>
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
    QSqlDatabase getNewDbConnection();
    int getid(QString username);
}



namespace FE {
    QMap<QStringList,QStringList> getFilesPathFormFileName(QString msg);

    QStringList getFileNames(QString dirname);

    bool processFileFromClient(QStringList filepaths);

    QStringList getLoadFile(QString neuron);


    QStringList writeArborAno(QString name,QString pos,QString swc,QString dir);
    QString cac_position(QString path);


}






#endif // DBFUNCTION_H
