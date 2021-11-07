#include "runorder.h"
#include "neuron_editing/neuron_format_converter.h"
const QStringList clienttypes={"TeraFly","TeraVR","TeraAI","HI5"};
double distance(const CellAPO &m1,const CellAPO &m2)
{
    return sqrt(
                (m1.x-m2.x)*(m1.x-m2.x)+
                (m1.y-m2.y)*(m1.y-m2.y)+
                (m1.z-m2.z)*(m1.z-m2.z)
            );
}
const int colorsize=21;
const int neuron_type_color[colorsize][3] = {
        {255, 255, 255},  // white,   0-undefined
        {20,  20,  20 },  // black,   1-soma
        {200, 20,  0  },  // red,     2-axon
        {0,   20,  200},  // blue,    3-dendrite
        {200, 0,   200},  // purple,  4-apical dendrite
        //the following is Hanchuan's extended color. 090331
        {0,   200, 200},  // cyan,    5
        {220, 200, 0  },  // yellow,  6
        {0,   200, 20 },  // green,   7
        {188, 94,  37 },  // coffee,  8
        {180, 200, 120},  // asparagus,	9
        {250, 100, 120},  // salmon,	10
        {120, 200, 200},  // ice,		11
        {100, 120, 200},  // orchid,	12
    //the following is Hanchuan's further extended color. 111003
    {255, 128, 168},  //	13
    {128, 255, 168},  //	14
    {128, 168, 255},  //	15
    {168, 255, 128},  //	16
    {255, 168, 128},  //	17
    {168, 128, 255}, //	18
        };

RunOrder::RunOrder(QString anoName)
{

    auto tmp=readSWC_file(anoName+".eswc");
    nt=NeuronTree__2__V_NeuronSWC_list(tmp);
    markers=readAPO_file(anoName+".apo");

    QFile orders(anoName);
    auto msgs=QString(orders.readAll()).split('\n');
    for(auto &msg:msgs){
        QRegExp msgreg("/(.*)_(.*):(.*)");
        if(msgreg.indexIn(msg)!=-1)
        {
            QString operationtype=msgreg.cap(1).trimmed();
//                bool isNorm=msgreg.cap(2).trimmed()=="norm";
            QString operatorMsg=msgreg.cap(3).trimmed();
            if(operationtype == "drawline" )
            {
                addline(operatorMsg);
            }
            else if(operationtype == "delline")
            {
                delline(operatorMsg);
            }
            else if(operationtype == "addmarker")
            {
                addmarker(operatorMsg);
            }
            else if(operationtype == "delmarker")
            {
                delmarker(operatorMsg);
            }
            else if(operationtype == "retypeline")
            {
                retypeline(operatorMsg);
            }
        }
    }

}

void RunOrder::addline(QString& msg)
{
    QStringList listwithheader=msg.split(';',Qt::SkipEmptyParts);
    if(listwithheader.size()<=1)
    {
        qDebug()<<"msg only contains header:"<<msg;
        return;
    }
    int from=0;
    QString username;

    {
        auto headerlist=listwithheader[0].split(' ',Qt::SkipEmptyParts);
        QString clienttype=headerlist[1].trimmed();
        for(int i=0;i<clienttypes.size();i++)
        {
            if(clienttypes[i]==clienttype)
            {
                from = i;
                break;
            }
        }
        username=headerlist[0].trimmed();
    }
    NeuronTree newTempNT=msg2V_NeuronSWC(listwithheader,username,from);
    nt.append(NeuronTree__2__V_NeuronSWC_list(newTempNT).seg[0]);
}
void RunOrder::delline(QString& msg)
{
    QStringList listwithheader=msg.split(';',Qt::SkipEmptyParts);
    if(listwithheader.size()<=1)
    {
        qDebug()<<"msg only contains header:"<<msg;
        return;
    }

    NeuronTree newTempNT=msg2V_NeuronSWC(listwithheader);
    auto seg=NeuronTree__2__V_NeuronSWC_list(newTempNT).seg[0];
    auto it=findseg(nt.seg.begin(),nt.seg.end(),seg);

    if(it!=nt.seg.end())
    {
        nt.seg.erase(it);
    }else
        qDebug()<<"not find delete line "<<msg;
}
void RunOrder::addmarker(QString& msg)
{
    QStringList listwithheader=msg.split(';',QString::SkipEmptyParts);
    if(listwithheader.size()<=1)
    {
        qDebug()<<"msg only contains header:"<<msg;
        return;
    }

    CellAPO marker;
    {
        QStringList markerPara=listwithheader[1].split(' ',QString::SkipEmptyParts);
        marker.x=markerPara[1].toFloat();
        marker.y=markerPara[2].toFloat();
        marker.z=markerPara[3].toFloat();
        int type= markerPara[0].toInt();
        marker.color.r=neuron_type_color[type][0];
        marker.color.g=neuron_type_color[type][1];
        marker.color.b=neuron_type_color[type][2];
    }
    wholePoint.push_back(marker);
}
void RunOrder::delmarker(QString& msg)
{
    QStringList listwithheader=msg.split(';',Qt::SkipEmptyParts);
    if(listwithheader.size()<=1)
    {
        qDebug()<<"msg only contains header:"<<msg;
        return;
    }

    CellAPO marker;
    {
        QStringList markerPara=listwithheader[1].split(' ',Qt::SkipEmptyParts);
        marker.x=markerPara[1].toFloat();
        marker.y=markerPara[2].toFloat();
        marker.z=markerPara[3].toFloat();
    }
    int index=-1;
    double threshold=10e-0;
    for(int i=0;i<markers.size();i++)
    {
        double dist=distance(marker,markers[i]);
        if(dist<threshold)
        {
            index=i;
        }
    }
    if(index>=0)
    {
       markers.removeAt(index);
    }else
    {
        qDebug()<<"failed to find marker to delete ,msg = "<<msg;
    }
}
void RunOrder::retypeline(QString& msg)
{
    QStringList listwithheader=msg.split(';',Qt::SkipEmptyParts);
    if(listwithheader.size()<=1)
    {
        qDebug()<<"msg only contains header:"<<msg;
        return;
    }


    int newtype=listwithheader[0].split(' ',Qt::SkipEmptyParts)[2].trimmed().toUInt();
    NeuronTree newTempNT=msg2V_NeuronSWC(listwithheader);
    auto seg=NeuronTree__2__V_NeuronSWC_list(newTempNT).seg[0];
//    qDebug()<<segments.seg.size();
    auto it=findseg(nt.seg.begin(),nt.seg.end(),seg);

    if(it!=nt.seg.end())
    {
        for(auto & unit:it->row)
        {
            unit.type=newtype;
        }
//        qDebug()<<"find retype line sucess "<<msg;
        return;
    }
    qDebug()<<"not find retype line "<<msg;
}
NeuronTree RunOrder::msg2V_NeuronSWC(QStringList &listwithheader,QString username,int from)
{
    NeuronTree newTempNT;
    newTempNT.listNeuron.clear();
    newTempNT.hashNeuron.clear();
    int cnt=listwithheader.size();
    int type=-1;

    for(int i=1;i<cnt;i++)
    {
        NeuronSWC S;
        QStringList nodelist=listwithheader[i].split(' ',Qt::SkipEmptyParts);
        if(nodelist.size()<4) return NeuronTree();
        S.n=i;
        type=nodelist[0].toUInt();
        S.type=type;
        S.x=nodelist[1].toFloat();
        S.y=nodelist[2].toFloat();
        S.z=nodelist[3].toFloat();
        S.r=username.toInt()*10+from;
        if(i==1) S.pn=-1;
        else S.pn=i-1;

        newTempNT.listNeuron.push_back(S);
        newTempNT.hashNeuron.insert(S.n,newTempNT.listNeuron.size());
    }
    return newTempNT;
}
vector<V_NeuronSWC>::iterator findseg(vector<V_NeuronSWC>::iterator begin,vector<V_NeuronSWC>::iterator end,const V_NeuronSWC seg)
{
    vector<V_NeuronSWC>::iterator result=end;
    double mindist=0.2;
    const int cnt=seg.row.size();

    while(begin!=end)
    {
        if(begin->row.size()==cnt)
        {
            double dist=0;
            for(int i=0;i<cnt;i++)
            {
                auto node=begin->row.at(i);
                dist+=sqrt(
                           pow(node.x-seg.row[i].x,2)
                          +pow(node.y-seg.row[i].y,2)
                          +pow(node.z-seg.row[i].z,2)
                           );
            }
            if(dist/cnt<mindist)
            {
                mindist=dist;
                result=begin;
            }

            dist=0;
            for(int i=0;i<cnt;i++)
            {
                auto node=begin->row.at(i);
                dist+=sqrt(
                           pow(node.x-seg.row[cnt-i-1].x,2)
                          +pow(node.y-seg.row[cnt-i-1].y,2)
                          +pow(node.z-seg.row[cnt-i-1].z,2)
                           );
            }
            if(dist/cnt<mindist)
            {
                mindist=dist;
                result=begin;
            }
        }
        begin++;
    }
    return result;
}


