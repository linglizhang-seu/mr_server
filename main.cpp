#include <QCoreApplication>
#include <iostream>
#include <basic_c_fun/basic_surf_objs.h>
#include <basic_c_fun/neuron_format_converter.h>
#include <unordered_map>
#include <unordered_set>
#include <QFile>

QString NeuronName="/Users/huanglei/Desktop/2.eswc";

void printV_NeuronSWC_List(V_NeuronSWC_list segs);
double length(V_NeuronSWC seg);
std::unordered_map<int,double> lengthPerUser(V_NeuronSWC_list segs);
NeuronTree colorNeuronByModality(NeuronTree nt);
std::vector<double> getBB(V_NeuronSWC seg);
std::unordered_set<int> getPeopleColWithBB(const NeuronTree nt,std::vector<double> BB);
NeuronTree colorNeuronWithColUsers(V_NeuronSWC_list segs,const NeuronTree nt);
void writeMap(QString name,std::unordered_map<int,double> hashmap);

int main(int argc, char *argv[])
{
    NeuronTree nt=readSWC_file(NeuronName);
    QString baseName=QString::fromStdString(NeuronName.toStdString().substr(NeuronName.lastIndexOf('/'),NeuronName.lastIndexOf('.')-NeuronName.lastIndexOf('/')));
    writeESWC_file(baseName+"_type.eswc",nt);
    V_NeuronSWC_list segs=NeuronTree__2__V_NeuronSWC_list(nt);
    //计算每个用户的标注的长度
    auto lengthMap=lengthPerUser(segs);
    writeMap(baseName+"_analyse.txt",lengthMap);
    //不同用户染色
    auto modalityNT=colorNeuronByModality(nt);
    writeESWC_file(baseName+"_modality.eswc",modalityNT);
    //热度图
    auto heatNT=colorNeuronWithColUsers(segs,nt);
    writeESWC_file(baseName+"_heat.eswc",heatNT);
    //计算

    return 0;
}

double length(V_NeuronSWC seg)
{
    double length=0.0;
    for(int i=1;i<seg.row.size();i++){
        length+=distL2square(seg.row[i],seg.row[i-1]);
    }
    return length;
}

std::unordered_map<int,double> lengthPerUser(V_NeuronSWC_list segs)
{
    std::unordered_map<int,double> lengthMap;
    for(auto seg:segs.seg){
        lengthMap[seg.row[0].r/10]+=length(seg);
    }
}

NeuronTree colorNeuronByModality(NeuronTree nt){
    for(auto &node:nt.listNeuron){
        node.type=int(node.r)%10;
    }
    return nt;
}

void printV_NeuronSWC_List(V_NeuronSWC_list segs){
    std::cout<<QString("n\ttype\tx\ty\tz\tr\tparent\tchild\tseg_id\tnodeinseg_id\n").toStdString();

    for(auto seg:segs.seg){
        std::cout<<"------------------------------------------------------------------------\n";

        for(auto node:seg.row){
            std::cout<<node.n<<"\t"<<node.type<<"\t"<<node.x<<"\t"<<node.y<<"\t"<<node.z
                    <<"\t"<<node.r<<"\t"<<node.parent<<"\t"<<node.nchild<<"\t"<<node.seg_id<<"\t"<<node.nodeinseg_id<<"\n";
        }
    }
}

std::vector<double> getBB(V_NeuronSWC seg){
    //x1,x2,y1,y2,z1,z2
    double x1,x2,y1,y2,z1,z2;
    x1=y1=z1=INT_MAX;
    x2=y2=z2=0;
    for(auto node:seg.row){
        if(x1>node.x)
            x1=node.x;
        if(x2<node.x)
            x2=node.x;
        if(y1>node.y)
            y1=node.y;
        if(y2<node.y)
            y2=node.y;
        if(z1>node.z)
            z1=node.z;
        if(z2<node.z)
            z2=node.z;
    }
    x1-=5;y1-=5;z1-=5;
    x2+=5;y2+=5;z2+=5;
    return {x1,x2,y1,y2,z1,z2};
}

std::unordered_set<int> getPeopleColWithBB(const NeuronTree nt,std::vector<double> BB){
    std::unordered_set<int> hashset;
    for(auto node:nt.listNeuron){
        if(node.x>BB[0]&&node.x<BB[1]
            &&node.y>BB[2]&&node.y<BB[3]
            &&node.z>BB[4]&&node.z<BB[5]){
            hashset.insert(int(node.r)/10);
        }
    }
    return hashset;
}

NeuronTree colorNeuronWithColUsers(V_NeuronSWC_list segs,const NeuronTree nt){
    for(auto &seg:segs.seg){
        auto BB=getBB(seg);
        int cnt=getPeopleColWithBB(nt,BB).size();
        for(auto &node:seg.row){
            node.type=cnt-1;
        }
    }
    return V_NeuronSWC_list__2__NeuronTree(segs);
}

void writeMap(QString name,std::unordered_map<int,double> hashmap){
    QFile f(name);
    QTextStream stream(&f);
    for(auto it=hashmap.begin();it!=hashmap.end();it++){
        stream<<it->first<<"\t"<<it->second<<"\n";
    }
    f.close();
}

