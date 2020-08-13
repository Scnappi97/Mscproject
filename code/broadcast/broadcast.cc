

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <map>
#include <tuple>
#include <iostream>
#include <functional>
#include <math.h>
#include <vector>

#include "hbeats_m.h"
#include "echos_m.h"

using namespace omnetpp;
using namespace std;

class fully_connected : public cSimpleModule
{
    private:
        int r;//round number
        map<string,string> mapSigs;
        hash<string> str_hash;
        int delr;
        simtime_t maxdelay;//the length of round r
        simtime_t startime;
        cMessage *roundMsg;
        int duration;//maximum round
        short hbeatkind = 10;
        short echokind = 20;
        short bcastkind = 30;
        tuple<int, string>Recho;//round,value
        map<string,map<string,Recho>>Rechomap;//pj <pi(round val)>
        tuple<string,string,int,string>bcastuple;//sig, pi,round,val
        vector<string>signedby;
        map<string,signedby>bcastmap;//target, signedby        //TODO ?
        tuple<string,int>deliver;//name,round
        map<string,deliver>delivermap;

        map<string, map<int,int>>countmap;//name,map
        map<string, int>statusmap;//0==crash
        map<string, int>crashmap;
        bool crashflag;


    public:
        fully_connected();
        virtual ~fully_connected();
    protected:
        virtual void forwardMessage(cMessage *msg, simtime_t delay);
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        virtual void sendhbeats(int hbeatR, simtime_t delay);
        virtual void sendecho(int echoR, simtime_t delay, string echoW);
        virtual void fully_connected::sendbroadcast(int round, string value);
        virtual simtime_t fully_connected::delaytime();
        virtual void fully_connected::delivermsg(string p, string val, string *sigs)
};

Define_Module(fully_connected);

fully_connected::fully_connected(){
    roundMsg = NULL;
}
fully_connected::~fully_connected(){
    cancelAndDelete(roundMsg);
}




void fully_connected::sendhbeats(int hbeatR, simtime_t delay,string name){
    char Msgname[10];
    sprintf(Msgname, " HB %s round %d",name.c_str(),hbeatR);
    hbeats *hbeatm = new hbeats(Msgname, hbeatkind);
    hbeatm->setHbeatname(name.c_str());
    hbeatm->setHbeatnum(hbeatR);//send hbeat round
    forwardMessage(hbeatm, delay);
}

void fully_connected::sendecho(simtime_t delay, string source, string val){
    char Msgname[10];
    sprintf(Msgname, "source: %s round : %d, value : %s ", source.c_str(), r, val);
    echos *echom = new echos(Msgname, 20);

    echom->setSourcename(echoW.c_str());
    echom->setRound(echoR);//send echo round
    echom->setValue(val);

    forwardMessage(echom, delay);
}

void fully_connected::sendbroadcast(int round, string value, string name){
    char Msgname[20];
    sprintf(Msgname,"broadcast: process %s, round %d, value %s", getName(), round, value.c_str());
    bcast *bcastm = new bcast(Msgname,30);
    bcastm->setSourcename(name.c_str());
    bcastm->setRound(round);
    bcastm->setValue(value.c_str());
    forwardMessage(bcastm,0);
}

void fully_connected::delivermsg(string p, string val){
    tuple<string, int>t(p,r);//pi and r
    delivermap[getName()]=t;
    simtime_t deltime = delaytime();
    char Msgname[20];
    sprintf(Msgname,"deliver: process %s,  value %s", p, value.c_str());//
    deliver *deliverm = new deliver(Msgname,40);
    deliverm->setSourcename(p);
    deliverm->setValue(val.c_str());
    forwardMessage(deliverm, deltime);
}


simtime_t fully_connected::delaytime(){
    simtime_t rectime = simTime();
    double p = (rectime - startime).dbl();
    double m = maxdelay.dbl();
    int t = p/m;
    simtime_t delaytime = startime + (simtime_t)((t+1)*m) - rectime;
    return deltime;
}



void fully_connected::initialize()// one time or every round?
{

    r = 0;
    delr = 3;
    maxdelay = 1.0;//length of a round
    duration = 5;//time window period
    startime = simTime();
    countmap[getName()][r]=0;
    statusmap[getName()]=1;
    crashmap[getName()]=15;
    crashflag = false;

    if(getName()=="node1"){
        string value = "vvvvvvvalueeeeeeee";
        sendbroadcast(r,value);
    }
    else{
        sendhbeats(r,0,getName());//at round r send hbeat r=0, delay =0
    }
    roundMsg = new cMessage("round over");
    scheduleAt(curtime + maxdelay, roundMsg);//send self round over
}


void fully_connected::handleMessage(cMessage *msg)
{
    if(msg==roundMsg){
        r = r+1;
        EV<<"round over"<<"\n";
        roundMsg = new cMessage("round over");
        scheduleAt(simTime()+maxdelay, roundMsg);
        if(r>duration){
            EV<<"*****************************round "<<r<<"\n";
            int rend = r - 1;
            int rstart = r - 1 - delr;
            int counts=0;
            for(int i=rstart;i<=rend;i++){
                counts+=countmap[getName()][i];
                EV<<"rstart "<<rstart<<" rend "<<rend<<"count"<<countmap[getName()][i]<<"\n";
            }
            EV<<"COUNTS"<<counts<<"\n";
            if(counts<3){
                bubble("crash self");
                EV<<"crash self\n";
                statusmap[getName()]=0;
                if(crashflag==false){
                    crashflag =true;
                    crashmap[getName()]=r;
                }
            }
        }
        sendhbeats(r,0,getName());

    delete msg;
      }
    if(msg->getKind()==hbeatkind){//receive the hbeats
            simtime_t deltime = delaytime();
            hbeats *hbeatm = check_and_cast<hbeats *>(msg);
            int hbeatR = hbeatm->getHbeatnum();
            string sourcename = hbeatm->getHbeatname();
            if(sourcename==getName()){
                    countmap[getName()][r]+=1;
                    bubble("get echo");
                    EV<<"get echo\n"<<"round"<<r;
                    EV<<getName()<< " COUNT "<< countmap[getName()][r] <<"\n";
                    delete msg;
            }else if(r <= (hbeatR + delr)){
                    bubble("receive hbeats");
                    EV<<getName()<<" receive hbeats at round "<<r<<"\n";
                    EV<<"content"<<": "<<sourcename<<" r: "<<hbeatR<<"\n";
                    sendhbeats(hbeatR,deltime,sourcename);
                    delete msg;
                     }
       }
        if(msg->getKind()==echokind){
            EV<<"get echo\n";
            echos *echom = check_and_cast<echos *>(msg);
            string name = echom->getSourcename();
            int round = echom->getRound();
            string val = echom->getValue();

            if(name == getName()){
                count[getIndex()]+=1;
                EV<< "count of echos"<<  count <<"\n";
                delete msg;
            }else if(Rechomap[getName()].find(name)!=Rechomap.end()){
                EV<<getName()<<"has delivered msg related to source"<<name<<"\n";
                if(bcastmap[name].size()>2){
                    delivermsg();//TODO
                }
            }else{
                //Todo check number of sigs
                    if(bcastmap[name].size()<=2){//2f
                        simtime_t deltime = delaytime();
                        if(r <= (round + delr)){sendecho(deltime,name,val); delete msg;}
                    }
            }
        }
        if(msg->getKind()==bcastkind){
            bcast *bcastm = check_and_cast<bcast *>(msg);
            string name = bcast->getSourcename();
            int round = bcast->getRound();
            string val = bcast->getValue();
            if(Rechomap[getName()].find(name)!=Rechomap.end()){//<pj,<pi,tuple>>
                EV<<"not first time"<<"\n";
            }else{//first time to receive a broadcast from "name"
                tuple<int, string>Rt(round,val);
                map<string,tuple>Rm;
                Rm[name]=Rt;
                Rechomap[getName()]=Rm;
                bcastuple=make_tuple("",name,round,val);
                //Todo sigs
                bcastmap[name]="";
                simtime_t deltime = delaytime();
                if(r <= (round + delr)){sendecho(deltime,name,val); delete msg;}
            }
        }
    }
}

void fully_connected::forwardMessage(cMessage *msg, simtime_t delay)
{

    int n = gateSize("out");
    cMessage *copymsg;
    EV<<"delay"<<delay<<"\n";
    for(int i=0; i<n; i=i+1)
    {
        copymsg = (cMessage *)msg->dup();
        sendDelayed(copymsg, delay, "out", i);
    }
    delete msg;
}


