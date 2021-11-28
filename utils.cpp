#include <QCoreApplication>
#include <QDir>
#include "neuron_editing/neuron_format_converter.h"
#include <hiredis.h>
#include "utils.h"


void dirCheck(QString dirBaseName)
{
    if(!QDir(QCoreApplication::applicationDirPath()+"/"+dirBaseName).exists())
    {
        QDir(QCoreApplication::applicationDirPath()).mkdir(dirBaseName);
    }
}

void setredis(int port,char *ano)
{
    redisContext *c = redisConnect("127.0.0.1", 6379);
    if (c == NULL || c->err) {
        if (c) {
            printf("Error: %s\n", c->errstr);
        } else {
            printf("Can't allocate redis context\n");
        }
    }
    redisReply *reply = (redisReply *)redisCommand(c, "SELECT 1");
    freeReplyObject(reply);
    reply=(redisReply *)redisCommand(c, "PERSIST %s",ano);
    if(reply->integer!=1){
        exit(-1);
    }
    freeReplyObject(reply);

    reply = (redisReply *)redisCommand(c, "SELECT 2");
    freeReplyObject(reply);
    reply=(redisReply *)redisCommand(c, "SET %d %s",port,ano);
    if(strcmp(reply->str,"OK")!=0){
        exit(-1);
    }
    freeReplyObject(reply);

    redisFree(c);

}

void expire(int port,char *ano)
{
    redisContext *c = redisConnect("127.0.0.1", 6379);
    if (c == NULL || c->err) {
        if (c) {
            printf("Error: %s\n", c->errstr);
        } else {
            printf("Can't allocate redis context\n");
        }
    }
    redisReply *reply = (redisReply *)redisCommand(c, "SELECT 1");
    freeReplyObject(reply);
    reply=(redisReply *)redisCommand(c, "EXPIRE %s 60",ano);
    freeReplyObject(reply);

    reply = (redisReply *)redisCommand(c, "SELECT 2");
    freeReplyObject(reply);
    reply=(redisReply *)redisCommand(c, "EXPIRE %d 60",port);
    freeReplyObject(reply);

    redisFree(c);
}

QStringList getSwcInBlock(const QString msg,const V_NeuronSWC_list& testVNL)
{
    /*
     * p1:brain_id;res;x;y;z;size;socket.descriptor
     * p2:Neuron_id/name
     * 返回：文件名，文件路径
     */
    QStringList paraList=msg.split(";",Qt::SkipEmptyParts);
    QString brain_id=paraList.at(0).trimmed();//1. tf name/RES  2. .v3draw// test:17302;RES;x;y;z;b
    //0: 18465/RESx18000x13000x5150
    //1: 12520
    //2: 7000
    //3: 2916
    int res=paraList.at(1).toInt();//>0
    int xpos=paraList.at(2).toInt();
    int ypos=paraList.at(3).toInt();
    int zpos=paraList.at(4).toInt();
    int blocksize=paraList.at(5).toInt();

    {
        QString name=brain_id;
        int x1=xpos-blocksize;
        int x2=xpos+blocksize;
        int y1=ypos-blocksize;
        int y2=ypos+blocksize;
        int z1=zpos-blocksize;
        int z2=zpos+blocksize;
        int cnt=pow(2,res-1);

        dirCheck("tmp");
        QString BBSWCNAME=QCoreApplication::applicationDirPath()+"/tmp/blockGet__"+name+QString("__%1__%2__%3__%4__%5__%6__%7.ano.eswc")
                .arg(x1).arg(x2).arg(y1).arg(y2).arg(z1).arg(z2).arg(cnt);
        x1*=cnt;x2*=cnt;y1*=cnt;y2*=cnt;z1*=cnt;z2*=cnt;
        V_NeuronSWC_list tosave;
        for(int i=0;i<testVNL.seg.size();i++)
        {
            NeuronTree SS;
            V_NeuronSWC seg_temp =  testVNL.seg.at(i);
            seg_temp.reverse();
            for(int j=0;j<seg_temp.row.size();j++)
            {
                if(seg_temp.row.at(j).x>=x1&&seg_temp.row.at(j).x<=x2
                        &&seg_temp.row.at(j).y>=y1&&seg_temp.row.at(j).y<=y2
                        &&seg_temp.row.at(j).z>=z1&&seg_temp.row.at(j).z<=z2)
                {
                    tosave.seg.push_back(seg_temp);
                    break;
                }
            }
        }
        qDebug()<<"get nt size:"<<tosave.seg.size();
        auto nt=V_NeuronSWC_list__2__NeuronTree(tosave);
        writeESWC_file(BBSWCNAME,nt);
        return {BBSWCNAME.right(BBSWCNAME.size()-BBSWCNAME.lastIndexOf('/')),BBSWCNAME};
    }
}

QStringList getApoInBlock(const QString msg,const QList <CellAPO>& wholePoint)
{
    /*
     * p1:brain_id;res;x;y;z;size;socket.descriptor
     * p2:Neuron_id/name
     * 返回：文件名，文件路径
     */
    QStringList paraList=msg.split(";",Qt::SkipEmptyParts);
    QString brain_id=paraList.at(0).trimmed();//1. tf name/RES  2. .v3draw// test:17302;RES;x;y;z;b
    //0: 18465/RESx18000x13000x5150
    //1: 12520
    //2: 7000
    //3: 2916
    int res=paraList.at(1).toInt();//>0
    int xpos=paraList.at(2).toInt();
    int ypos=paraList.at(3).toInt();
    int zpos=paraList.at(4).toInt();
    int blocksize=paraList.at(5).toInt();

    {
        QString name=brain_id;
        int x1=xpos-blocksize;
        int x2=xpos+blocksize;
        int y1=ypos-blocksize;
        int y2=ypos+blocksize;
        int z1=zpos-blocksize;
        int z2=zpos+blocksize;
        int cnt=pow(2,res-1);


        dirCheck("tmp");
        QString BBAPONAME=QCoreApplication::applicationDirPath()+"/tmp/blockGet__"+name+QString("__%1__%2__%3__%4__%5__%6__%7.ano.apo")
                .arg(x1).arg(x2).arg(y1).arg(y2).arg(z1).arg(z2).arg(cnt);
        x1*=cnt;x2*=cnt;y1*=cnt;y2*=cnt;z1*=cnt;z2*=cnt;
        qDebug()<<"x1,x2,y1,y2,z1,z2"<<x1<<x2<<y1<<y2<<z1<<z2;
        QList <CellAPO> to_save;
        for(auto marker:wholePoint)
        {
            if(marker.x>=x1&&marker.x<=x2
              &&marker.y>=y1&&marker.y<=y2
              &&marker.z>=z1&&marker.z<=z2)
            {
                to_save.append(marker);
            }
        }
        qDebug()<<"to_save.size()="<<to_save.size();
        writeAPO_file(BBAPONAME,to_save);
        return {BBAPONAME.right(BBAPONAME.size()-BBAPONAME.lastIndexOf('/')),BBAPONAME};
    }
}
