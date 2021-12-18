#ifndef ANALYSESWC_H
#define ANALYSESWC_H
#include <QString>
#include <QVector3D>
#include <basic_c_fun/basic_surf_objs.h>
#include <neuron_editing/neuron_format_converter.h>
#include "utils.h"

void doaddusertypr(QString inswc,QString outswc)
{
    auto nt=readSWC_file(inswc);
    for(auto &node:nt.listNeuron){
        node.type=node.r/10;
    }
    writeESWC_file(outswc,nt);
}

void docheckusertype(QString inswc,QString outswc)
{
    auto nt=readSWC_file(inswc);
    for(auto &node:nt.listNeuron){
        if(node.creatmode/10==0){
            qDebug()<<node.n;
        }
        node.type=node.creatmode/10;
    }
    writeESWC_file(outswc,nt);
}

void domodiltytype(QString inswc,QString outswc)
{
    auto nt=readSWC_file(inswc);
    for(auto &node:nt.listNeuron){
        node.type=int(node.r)%10*2+int(node.creatmode)%10+3;
    }
    writeESWC_file(outswc,nt);
}

void getadduserlength(QString inswc,QString lengthfile)
{
    auto nt=readSWC_file(inswc);
    for(auto &node:nt.listNeuron){
        node.type=node.r/10;
    }
    auto segments=NeuronTree__2__V_NeuronSWC_list(nt);

    QMap<int,double> hashmap;
    for(auto &seg:segments.seg){
        hashmap[int(seg.row[0].type)]+=getsegmentlength(seg);
    }
    QFile f(lengthfile);
    if(f.open(QIODevice::WriteOnly)){
        auto keys=hashmap.keys();
        QString data=QString("%1:%2\n").arg("usertype").arg("length");
        f.write(data.toStdString().c_str(),data.size());
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
        node.type=node.creatmode/10;
    }
    auto segments=NeuronTree__2__V_NeuronSWC_list(nt);

    QMap<int,double> hashmap;
    for(auto &seg:segments.seg){
        hashmap[int(seg.row[0].type)]+=getsegmentlength(seg);
    }
    QFile f(lengthfile);
    if(f.open(QIODevice::WriteOnly)){
        auto keys=hashmap.keys();
        QString data=QString("%1:%2\n").arg("usertype").arg("length");
        f.write(data.toStdString().c_str(),data.size());
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
    users.insert(int(seg.row.at(0).r/10));
    users.insert(int(seg.row.at(0).creatmode/10));

    auto BBox=getBBox(seg);
    for(auto &seg:sges.seg){
        if(isInBBox(BBox,seg)){
            users.insert(int(seg.row.at(0).r/10));
            users.insert(int(seg.row.at(0).creatmode/10));
        }
    }

    for(auto &node:seg.row){
        node.type=users.size();
    }
}

//void doheatmap(QString inswc,QString outswc)
//{
//    auto nt=readSWC_file(inswc);
//    auto segs=NeuronTree__2__V_NeuronSWC_list(nt);

//    for(auto &seg:segs.seg){
//        dosegheatmap(seg,segs);
//    }
//    writeESWC_file(outswc,V_NeuronSWC_list__2__NeuronTree(segs));
//}

void donodeheatmap(QString inswc,QString outswc)
{
    auto nt=readSWC_file(inswc);
    const auto cnt=readSWC_file(inswc);
    int halflen=20;
    for(auto &node:nt.listNeuron){
        int x1=node.x-halflen;
        int x2=node.x+halflen;
        int y1=node.y-halflen;
        int y2=node.y+halflen;
        int z1=node.z-halflen;
        int z2=node.z+halflen;
        QSet<int> myset;
        myset.insert(int(node.r/10));
        myset.insert(int(node.creatmode/10));

        for(auto &n:cnt.listNeuron){
            if(n.x>x1&&n.x<x2
                    &&n.y>y1&&n.y<y2
                    &&n.z>z1&&n.z<z2){
                myset.insert(int(n.r/10));
                myset.insert(int(n.creatmode/10));
            }
        }
        node.type=myset.size()+3;
    }
    writeESWC_file(outswc,nt);
}

V_NeuronSWC compare(QString file1,QString file2)
{
    auto nt1=readSWC_file(file1);
    auto nt2=readSWC_file(file2);

    if(nt1.listNeuron.size()!=nt2.listNeuron.size())
        qDebug()<<"Fatal:nt.listneuron size !=";
    auto segs1=NeuronTree__2__V_NeuronSWC_list(nt1);
    auto segs2=NeuronTree__2__V_NeuronSWC_list(nt2);
    if(segs1.seg.size()!=segs2.seg.size())
        qDebug()<<"Fatal:segs.seg size !=";
    for(auto seg:segs1.seg)
    {

        auto it=findseg(segs2.seg.begin(),segs2.seg.end(),seg);
        if(seg.row.at(0).creatmode==0.0)
        {
            qDebug()<<it->row.at(0).x<<" "<<it->row.at(0).y<<" "<<it->row.at(0).z;
            return *it;
        }
        if(it!=segs2.seg.end()&&it->row.at(0).type==seg.row.at(0).type)
        {
            segs2.seg.erase(it);
        }

        else{
            qDebug()<<"Fatal:seg1.seg not find in seg2";
            qDebug()<<(it!=segs2.seg.end())<<"\t"<<(it->row.at(0).type==seg.row.at(0).type);
        }
    }
}

void douserproof(QString insec,QString outswc)
{
    auto nt=readSWC_file(insec);
    for(auto &node:nt.listNeuron)
    {
        node.type=int(node.r)/10==20?3:2;
    }
    writeESWC_file(outswc,nt);
}

std::pair<double,double> doproofdetaills(QString raw,QString proof,QString outswc)
{
    auto rawnt=readSWC_file(raw);
    auto proofnt=readSWC_file(proof);

    auto rawsegs=NeuronTree__2__V_NeuronSWC_list(rawnt);
    auto proofsegs=NeuronTree__2__V_NeuronSWC_list(proofnt);

    double diffraw2proof=0;//删除的结构
    double diffproof2raw=0;//增加的结构

    for(auto &seg:rawsegs.seg){
        auto it=findseg(proofsegs.seg.begin(),proofsegs.seg.end(),seg);
        if(it==proofsegs.seg.end()||((int)it->row.at(0).r/10)!=((int)seg.row.at(0).r/10))
        {
            //找不到或者找到了但增加的用户不同
            diffraw2proof+=getsegmentlength(seg);
            //改变原始的
            for(auto &row:seg.row)
                row.type=4;
        }
    }

    for(auto &seg:proofsegs.seg){
        auto it=findseg(rawsegs.seg.begin(),rawsegs.seg.end(),seg);
        if(it==rawsegs.seg.end()||((int)it->row.at(0).r/10)!=((int)seg.row.at(0).r/10))
        {
            //找不到或者找到了但增加的用户不同
            diffproof2raw+=getsegmentlength(seg);
            for(auto &row:seg.row)
                row.type=5;
            rawsegs.seg.push_back(seg);
            //附加到原始的nt
        }
    }
    writeESWC_file(outswc,V_NeuronSWC_list__2__NeuronTree(rawsegs));
    return {diffraw2proof,diffproof2raw};
}

void doproof(QStringList inrawfiles,QStringList inproofedfiles,QStringList outlist,QString out)
{
    if(inrawfiles.size()!=inproofedfiles.size()) return;
    QFile f(out);
    if(!f.open(QIODevice::WriteOnly)) return;
    for(int i=0;i<inrawfiles.size();i++){
        auto p=doproofdetaills(inrawfiles[i],inproofedfiles[i],outlist[i]);
        QString data=QString("%1--%2:%3 %4").arg(inrawfiles[i]).arg(inproofedfiles[i]).arg(p.first).arg(p.second);
        f.write(data.toStdString().c_str(),data.size());
    }
    f.close();
}

void doselfconform(QString inswc,QString outswc)
{
    auto nt=readSWC_file(inswc);
    for(auto &n:nt.listNeuron){
        n.type=(int(n.r/10)==int(n.creatmode/10))?2:4;
    }
    writeESWC_file(outswc,nt);
}

void mergeNts(QStringList swcs,QString outswc)
{
    V_NeuronSWC_list segs;
    const int offset=2;
    const int size=swcs.size();

    for(int i=0;i<size;i++)
    {
        auto nt=readSWC_file(swcs[i]);
        qDebug()<<nt.listNeuron.size();
        for(auto &n:nt.listNeuron){
            n.type=i+offset;
        }
        auto segments=NeuronTree__2__V_NeuronSWC_list(nt);
        segs.seg.insert(segs.seg.end(),segments.seg.begin(),segments.seg.end());
    }
    auto nt=V_NeuronSWC_list__2__NeuronTree(segs);
    qDebug()<<nt.listNeuron.size();
    writeESWC_file(outswc,nt);
}





#endif // ANALYSESWC_H
