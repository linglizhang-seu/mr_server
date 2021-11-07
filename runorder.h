#ifndef RUNORDER_H
#define RUNORDER_H

#include <QString>
#include "basic_c_fun/basic_surf_objs.h"
#include "neuron_editing/v_neuronswc.h"
class RunOrder
{
public:
    RunOrder(QString anoName);
    void addline(QString&);
    void delline(QString&);
    void addmarker(QString&);
    void delmarker(QString&);
    void retypeline(QString&);
private:
    NeuronTree msg2V_NeuronSWC(QStringList &listwithheader,QString username=0,int from=0);
    vector<V_NeuronSWC>::iterator findseg(vector<V_NeuronSWC>::iterator begin,vector<V_NeuronSWC>::iterator end,const V_NeuronSWC seg);
    V_NeuronSWC_list nt;
    QList<CellAPO> markers;

};

#endif // RUNORDER_H
