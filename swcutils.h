#ifndef SWCUTILS_H
#define SWCUTILS_H
#include "basic_c_fun/basic_surf_objs.h"
#include "neuron_editing/neuron_format_converter.h"
#include "neuron_editing/v_neuronswc.h"
#include <QVector3D>
#include "utils.h"
extern int distthres;
extern int lengththres;
void retype(V_NeuronSWC_list &segs,int type)
{
    for(auto &seg:segs.seg){
        for(auto &node:seg.row){
            node.type=type;
        }
    }
}

void retype(V_NeuronSWC &seg,int type)
{
        for(auto &node:seg.row){
            node.type=type;
        }

}

double distance2ine(const NeuronSWC &node,const V_NeuronSWC_unit &u1,const V_NeuronSWC_unit &u2)
{
      QVector3D v;v.setX(node.x);v.setY(node.y);v.setZ(node.z);
      QVector3D v1;v1.setX(u1.x);v1.setY(u1.y);v1.setZ(u1.z);
      QVector3D v2;v2.setX(u2.x);v2.setY(u2.y);v2.setZ(u2.z);

      QVector3D line=v1-v2;line=line.normalized();
      return v.distanceToLine(v1,line);
}

bool IsInSegments(const NeuronSWC& node,const V_NeuronSWC_list &segs)
{

    double mindist=INT_MAX;
    double dist;
    for(auto &seg:segs.seg){
        int size=seg.row.size();
        for(int i=1;i<size;i++){
            dist=distance2ine(node,seg.row[i-1],seg.row[i]);
            mindist=min(mindist,dist);
        }
    }
    return mindist<distthres;
}

bool istype(const V_NeuronSWC seg,int type)
{
    for(auto &node:seg.row)
    {
        if(node.type!=type) return false;
    }
    return true;
}

std::vector<V_NeuronSWC_list> comapreA2B(QString swc1,QString swc2,QString out)
{
    //读取swc
    auto nt1=readSWC_file(swc1);
    auto nt2=readSWC_file(swc2);

    //将swc1设置为蓝色色
    for(auto &node:nt1.listNeuron){
        node.type=3;
    }

    const auto segs=NeuronTree__2__V_NeuronSWC_list(nt2);
    //将swc1中在swc2中找不到的点设置为红色
    for(auto &node:nt1.listNeuron){
        if(!IsInSegments(node,segs))
        {
            node.type=2;
        }
    }

//    writeESWC_file("1"+out,nt1);
    V_NeuronSWC_list only;//不在2中
    V_NeuronSWC_list total;//全在2中
    V_NeuronSWC_list part;//部分在2中
    auto segs1=NeuronTree__2__V_NeuronSWC_list(nt1);

    for(auto &seg:segs1.seg){
        if(istype(seg,2)){
            //完全未找到的结构
            only.seg.push_back(seg);
        }else if(istype(seg,3)){
            //完全找到的结构
            total.seg.push_back(seg);
        }else {
            //部分找到的结构
            part.seg.push_back(seg);
        }
    }

    retype(only,2);
    retype(total,3);

    for(auto &seg:part.seg){
        int cnt=seg.row.size();
        int type=seg.row[0].type;
        //判断颜色是不是头尾分布
        int i;
        //找到第一个颜色变动的位置
        for(i=1;i<cnt;i++){
            if(seg.row[i].type!=type) break;
        }

        int tmp=i;//缓存第一个颜色变更的idx
        QSet<int> set;
        for(;i<cnt;i++){
            set.insert(seg.row[i].type);
        }
        if(set.size()==1)
        {
            V_NeuronSWC seg1,seg2;
            {
                seg1.row.insert(seg1.row.end(),seg.row.begin(),seg.row.begin()+tmp+1);
                for(int i=0;i<seg1.row.size();i++)
                {
                    seg1.row[i].n=i+1;
                    seg1.row[i].parent=i+2;
                }
                seg1.row.back().parent=-1;
            }

            {
                seg2.row.insert(seg2.row.end(),seg.row.begin()+tmp,seg.row.end());
                for(int i=0;i<seg2.row.size();i++)
                {
                    seg2.row[i].n=i+1;
                    seg2.row[i].parent=i+2;
                }
                seg2.row.back().parent=-1;
            }

            if(type==2){//头部是2号色，独有的结构
                if(getsegmentlength(seg1)<lengththres){//独有太短
                    //太短了，归共有
                    seg2.row.insert(seg2.row.begin(),seg1.row.begin(),seg1.row.end());
                    for(int i=0;i<seg2.row.size();i++)
                    {
                        seg2.row[i].n=i+1;
                        seg2.row[i].parent=i+2;
                    }
                    seg2.row.back().parent=-1;
                    total.seg.push_back(seg2);
                }else if(getsegmentlength(seg2)<lengththres){//共有太短
                    seg1.row.insert(seg1.row.end(),seg2.row.begin(),seg2.row.end());
                    for(int i=0;i<seg1.row.size();i++)
                    {
                        seg1.row[i].n=i+1;
                        seg1.row[i].parent=i+2;
                    }
                    seg1.row.back().parent=-1;
                    only.seg.push_back(seg1);
                }else{
                    only.seg.push_back(seg1);
                    total.seg.push_back(seg2);
                }
            }else{
                //头部是3号色，共有的结构
                if(getsegmentlength(seg1)<lengththres){//共有太短
                    //太短了，归独有
                    seg2.row.insert(seg2.row.begin(),seg1.row.begin(),seg1.row.end());
                    for(int i=0;i<seg2.row.size();i++)
                    {
                        seg2.row[i].n=i+1;
                        seg2.row[i].parent=i+2;
                    }
                    seg2.row.back().parent=-1;
                    only.seg.push_back(seg2);
                }else if(getsegmentlength(seg2)<lengththres){//独有太短
                    //太短了，归共有
                    seg1.row.insert(seg1.row.end(),seg2.row.begin(),seg2.row.end());
                    for(int i=0;i<seg1.row.size();i++)
                    {
                        seg1.row[i].n=i+1;
                        seg1.row[i].parent=i+2;
                    }
                    seg1.row.back().parent=-1;
                    total.seg.push_back(seg1);
                }else{
                    only.seg.push_back(seg2);
                    total.seg.push_back(seg1);
                }
            }
        }else{
            double len2=0,len3=0;
            for(int i=1;i<seg.row.size();i++){
                auto prenode=seg.row.at(i-1);
                auto curnode=seg.row.at(i);
                if(curnode.type==2) len2+=sqrt(pow(curnode.x-prenode.x,2)+pow(curnode.y-prenode.y,2)+pow(curnode.z-prenode.z,2));
                else len3+=sqrt(pow(curnode.x-prenode.x,2)+pow(curnode.y-prenode.y,2)+pow(curnode.z-prenode.z,2));
            }
            if(len2>len3) only.seg.push_back(seg);
            else total.seg.push_back(seg);
        }
    }
    retype(only,2);
    retype(total,3);
    auto segss=only;
    segss.seg.insert(segss.seg.end(),total.seg.begin(),total.seg.end());
    writeESWC_file(QString("%1_%2"+out).arg(distthres).arg(lengththres),V_NeuronSWC_list__2__NeuronTree(segss));
    return {only,total};
}



void compareA2Bv2(QString swc1,QString swc2,QString out)
{
            auto res12=comapreA2B(swc1,swc2,"_12.eswc");//我的重建独有的(需要删除的)，共有的
//            for(auto it=res12[0].seg.begin();it!=res12[0].seg.end();){
//                if(getsegmentlength(*it)<lengththres){
//                    res12[1].seg.push_back(*it);
//                    it=res12[0].seg.erase(it);
//                }else{
//                    it++;
//                }
//            }

            auto res21=comapreA2B(swc2,swc1,"_21.eswc");//标准答案独有的(需要增加的)，共有的
//            for(auto it=res21[0].seg.begin();it!=res21[0].seg.end();){
//                if(getsegmentlength(*it)<lengththres){
//                    res21[1].seg.push_back(*it);
//                    it=res21[0].seg.erase(it);
//                }else{
//                    it++;
//                }
//            }

            retype(res12[0],4);
            retype(res12[1],3);
            retype(res21[0],5);
            retype(res21[1],6);

            qDebug()<<getsegmentslength(res21[0]);

            V_NeuronSWC_list segs;
            segs.seg.insert(segs.seg.end(),res12[0].seg.begin(),res12[0].seg.end());
            segs.seg.insert(segs.seg.end(),res12[1].seg.begin(),res12[1].seg.end());//蓝色
            segs.seg.insert(segs.seg.end(),res21[0].seg.begin(),res21[0].seg.end());
            for(auto it=segs.seg.begin();it!=segs.seg.end();){
                if(it->row.size()<=1){
                    it=segs.seg.erase(it);
                }else {
                    ++it;
                }
            }
          writeESWC_file(out,V_NeuronSWC_list__2__NeuronTree(segs));
          writeESWC_file("shouldadd_"+out,V_NeuronSWC_list__2__NeuronTree(res21[0]));
}


#endif // SWCUTILS_H
