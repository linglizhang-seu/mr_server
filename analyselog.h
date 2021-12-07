#ifndef ANALYSELOG_H
#define ANALYSELOG_H

#include <QFile>

QStringList readorders(QString infile)
{
    const int thirdId=20;
    QFile f(infile);
    if(!f.open(QIODevice::ReadOnly)){
        return {};
    }

    auto orders=f.readAll().split('\n');
    QStringList res;
    for(auto array:orders)
        res.push_back(QString(array));
    return res;
}


void getUnUse(QStringList orders)
{

}
#endif // ANALYSELOG_H
