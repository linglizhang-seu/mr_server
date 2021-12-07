#ifndef ANALYSESWC_H
#define ANALYSESWC_H
#include <QString>
#include <basic_c_fun/basic_surf_objs.h>
#include <neuron_editing/neuron_format_converter.h>
void doaddusertypr(QString inswc,QString outswc)
{
    auto nt=readSWC_file(inswc);
    for(auto &node:nt.listNeuron){
        node.type=node.r/10+1;
    }
    writeESWC_file(outswc,nt);
}

void docheckusertype(QString inswc,QString outswc)
{
    auto nt=readSWC_file(inswc);
    for(auto &node:nt.listNeuron){
        node.type=node.creatmode/10+1;
    }
    writeESWC_file(outswc,nt);
}

void domodiltytype(QString inswc,QString outswc)
{
    auto nt=readSWC_file(inswc);
    for(auto &node:nt.listNeuron){
        node.type=int(node.r)%10*2+int(node.creatmode)%10+1;
    }
    writeESWC_file(outswc,nt);
}

double getsegmentlength(const V_NeuronSWC &seg)
{
    const int cnt=seg.row.size();
    double length=0;

    for(int i=1;i<cnt;i++){
        auto prenode=seg.row.at(i-1);
        auto curnode=seg.row.at(i);
        length+=sqrt(pow(curnode.x-prenode.x,2)+pow(curnode.y-prenode.y,2)+pow(curnode.z-prenode.z,2));
    }
    return length;
}

void getadduserlength(QString inswc,QString lengthfile)
{
    auto nt=readSWC_file(inswc);
    for(auto &node:nt.listNeuron){
        node.type=node.r/10+1;
    }
    auto segments=NeuronTree__2__V_NeuronSWC_list(nt);

    QMap<int,double> hashmap;
    for(auto &seg:segments.seg){
        hashmap[int(seg.row[0].type)]+=getsegmentlength(seg);
    }
    QFile f(lengthfile);
    if(f.open(QIODevice::WriteOnly)){
        auto keys=hashmap.keys();
        for(auto &key:keys){
            QString data=QString("%1:%2\n").arg(key).arg(hashmap.value(key));
            f.write(data.toStdString().c_str(),data.size());
        }
        f.close();
    }
}

void getretypeuserlength(QString inswc,QString lengthfile)
{
    auto nt=readSWC_file(inswc);
    for(auto &node:nt.listNeuron){
        node.type=node.creatmode/10+1;
    }
    auto segments=NeuronTree__2__V_NeuronSWC_list(nt);

    QMap<int,double> hashmap;
    for(auto &seg:segments.seg){
        hashmap[int(seg.row[0].type)]+=getsegmentlength(seg);
    }
    QFile f(lengthfile);
    if(f.open(QIODevice::WriteOnly)){
        auto keys=hashmap.keys();
        for(auto &key:keys){
            QString data=QString("%1:%2\n").arg(key).arg(hashmap.value(key));
            f.write(data.toStdString().c_str(),data.size());
        }
        f.close();
    }
}

std::array<int,6> getBBox(const V_NeuronSWC &seg)
{
    int x1,x2,y1,y2,z1,z2;
    x1=y1=z1=INT_MAX;
    x2=y2=z2=0;
    for(auto &node:seg.row){
        if(node.x<x1) x1=node.x;
        if(node.x>x2) x2=node.x;

        if(node.y<y1) y1=node.y;
        if(node.y>y2) y2=node.y;

        if(node.z<z1) z1=node.z;
        if(node.z>z2) z2=node.z;
    }
    return {x1,x2,y1,y2,z1,z2};
}

bool isInBBox(const std::array<int,6> &bbox,const V_NeuronSWC &seg)
{
    int cntNodeInBBox=0;
    for(auto &node:seg.row){
        if(
                node.x>=bbox[0]&&node.x<=bbox[1]
              &&node.y>=bbox[2]&&node.y<=bbox[3]
              &&node.z>=bbox[4]&&node.z<=bbox[5]
                )
            cntNodeInBBox++;
    }
    if(1.0*cntNodeInBBox/seg.row.size()>=0.3) return true;
    return false;
}

void dosegheatmap(V_NeuronSWC &seg,const V_NeuronSWC_list &sges)
{
    QSet<int> users;
    auto BBox=getBBox(seg);

    for(auto &seg:sges.seg){
        if(isInBBox(BBox,seg)){
            users.insert(int(seg.row.at(0).r/10));
            users.insert(int(seg.row.at(0).creatmode/10));
        }
    }

    for(auto &node:seg.row){
        node.type=users.size()+1;
    }
}

void doheatmap(QString inswc,QString outswc)
{
    auto nt=readSWC_file(inswc);
    auto segs=NeuronTree__2__V_NeuronSWC_list(nt);

    for(auto &seg:segs.seg){
        dosegheatmap(seg,segs);
    }
    writeESWC_file(outswc,V_NeuronSWC_list__2__NeuronTree(segs));
}


#endif // ANALYSESWC_H
