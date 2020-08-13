/*
 * proof_of_life.cc
 *
 *  Created on: 2020Äê7ÔÂ8ÈÕ
 *      Author: 13671
 */

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <map>
#include <iostream>
#include <functional>
#include <math.h>

#include "hbeats_m.h"

using namespace omnetpp;
using namespace std;

class fully_connected : public cSimpleModule
{
    private:
        int r;//round number
        int delr;

        simtime_t maxdelay;//the length of round r
        simtime_t startime;
        cMessage *roundMsg;
        int duration;//maximum round
        short hbeatkind = 10;
        short echokind = 20;
        map<string, map<int,int>>countmap;//name,map<round, echo count>
        map<string, int>statusmap;//<name, status>0==crash
        map<string,vector<string>>valuemap;//name value
        cOutVector CountVector;

        map<string, int>crashmap;//<name, round> crash round

        bool crashflag;

    public:
        fully_connected();
        virtual ~fully_connected();
    protected:
        virtual void forwardMessage(cMessage *msg, simtime_t delay);
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        virtual void sendhbeats(int hbeatR, simtime_t delay,string name,string val);
        virtual simtime_t delaytime();
        virtual void finish() override;

};

Define_Module(fully_connected);

fully_connected::fully_connected(){
    roundMsg = NULL;
}
fully_connected::~fully_connected(){
    cancelAndDelete(roundMsg);
}




void fully_connected::sendhbeats(int hbeatR, simtime_t delay,string name,string val){
    char Msgname[10];
    sprintf(Msgname, " Bcasting %s round %d",name.c_str(),hbeatR);
    hbeats *hbeatm = new hbeats(Msgname, hbeatkind);
    hbeatm->setHbeatname(name.c_str());
    hbeatm->setHbeatnum(hbeatR);//send hbeat round
    hbeatm->setVal(val.c_str());
    forwardMessage(hbeatm, delay);
}


simtime_t fully_connected::delaytime(){
    simtime_t rectime = simTime();
    double p = (rectime - startime).dbl();
    double m = maxdelay.dbl();
    int t = p/m;
    simtime_t delaytime = startime + (simtime_t)((t+1)*m) - rectime;
    return delaytime;
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


    WATCH(countmap[getName()][r]);
    CountVector.setName("CountEchos");

    EV<<"sending broadcast"<<"\n";

    if(strcmp(getName(),"node1")==0){sendhbeats(r,0,getName(),"*****value1*****");}
    if(strcmp(getName(),"node2")==0){sendhbeats(r,0,getName(),"*****value2*****");}
    if(strcmp(getName(),"node3")==0){sendhbeats(r,0,getName(),"*****value3*****");}
    if(strcmp(getName(),"node4")==0){sendhbeats(r,0,getName(),"*****value4*****");}

    roundMsg = new cMessage("round over");
    scheduleAt(startime + maxdelay, roundMsg);//send self round over
}


void fully_connected::handleMessage(cMessage *msg)
{
    //if(statusmap[getName()]==0){cancelAndDelete(msg);}
    //else
    if(msg==roundMsg){// send hbeat or send echo
    r = r+1;
    EV<<"round over round = "<<r<<"\n";
    roundMsg = new cMessage("round over");

    EV<<"countmap"<<countmap[getName()][r-1]<<"\n";
    CountVector.record(countmap[getName()][r-1]);

    scheduleAt(simTime()+maxdelay, roundMsg);
        if(r>duration){
            EV<<"*************round**************** "<<r<<"\n";
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
        sendhbeats(r,0,getName(),"");

    delete msg;
  }else if(uniform(0,1)<0){//strcmp(getName(),"node1")==0&&
        EV << "Losing message"<<" name "<<strcmp(getName(),"node1");
        bubble("message lost");
        delete msg;
    }
    else{
        if(msg->getKind()==hbeatkind){//receive the hbeats
                simtime_t deltime = delaytime();
                hbeats *hbeatm = check_and_cast<hbeats *>(msg);
                int hbeatR = hbeatm->getHbeatnum();
                string sourcename = hbeatm->getHbeatname();
                string val = hbeatm->getVal();
                if(sourcename==getName()){
                    countmap[getName()][r]+=1;
                    bubble("get echo");
                    EV<<"get echo\n"<<"round"<<r;
                    EV<<getName()<< " COUNT "<< countmap[getName()][r] <<"\n";
                    delete msg;
                }else if(r <= (hbeatR + delr)){
                    bubble("get broadcast");
                    EV<<getName()<<" get broadcast at round "<<r<<"\n";
                    EV<<"content"<<": "<<sourcename<<" r: "<<hbeatR<<"value:"<<val<<"\n";
                    if(crashflag==false){ valuemap[getName()].push_back(val); }
                    EV<<"vec size"<<valuemap[getName()].size()<<"\n";
                    sendhbeats(hbeatR,deltime,sourcename,val);
                    delete msg;
                }
       }
    }
}

void fully_connected::forwardMessage(cMessage *msg, simtime_t delay)
{

    int n = gateSize("out");
    cMessage *copymsg;
    //EV<<"delay"<<delay<<"\n";
    for(int i=0; i<n; i=i+1)
    {
        copymsg = (cMessage *)msg->dup();
        //EV << "Forwarding message " << copymsg << " on port out[" << i << "]\n";
        sendDelayed(copymsg, delay, "out", i);
    }
    delete msg;
}
void fully_connected::finish(){
    //recordScalar("#status", crashmap[getName()]);
    recordScalar("#valuemap",valuemap[getName()].size());

}

