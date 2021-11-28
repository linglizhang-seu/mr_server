#ifndef UTILS_H
#define UTILS_H

#include <QCoreApplication>
#include <QDir>
#include "neuron_editing/neuron_format_converter.h"
#include <hiredis.h>

void dirCheck(QString dirBaseName);
void setredis(int port,char *ano);
void expire(int port,char *ano);
QStringList getSwcInBlock(const QString msg,const V_NeuronSWC_list& testVNL);
QStringList getApoInBlock(const QString msg,const QList <CellAPO>& wholePoint);
#endif // UTILS_H
