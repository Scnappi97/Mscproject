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
#include "echos_m.h"

using namespace omnetpp;
using namespace std;

class fully_connected : public cSimpleModule
{
    private:
        int r;//round number
        map<string,string> mapSigs;
        //map<string,int> receivermap;
        hash<string> str_hash;
        int delr;
        int count[10];//count echos
        simtime_t maxdelay;//the length of round r
        simtime_t curtime;
        cMessage *roundMsg;
        int duration;//maximum round
        short hbeatkind = 10;

    public:
        fully_connected();
        virtual ~fully_connected();
    protected:
        virtual void forwardMessage(cMessage *msg, simtime_t delay);
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        //virtual cMessage *generateMsg();
        //virtual cMessage *Copymsg(cMessage *msg);
        //virtual int checkhbeats(cMessage *msg);
        virtual void sendhbeats(int hbeatR, simtime_t delay);
        virtual void sendecho(int echoR, simtime_t delay, string echoW);

};

Define_Module(fully_connected);

fully_connected::fully_connected(){
    roundMsg = NULL;
}
fully_connected::~fully_connected(){
    cancelAndDelete(roundMsg);
}




void fully_connected::sendhbeats(int hbeatR, simtime_t delay){
    char Msgname[10];
    sprintf(Msgname, "heartbeats of  %s",getName());
    hbeats *hbeatm = new hbeats(Msgname, hbeatkind);
    hbeatm->setHbeatname(getName());
    hbeatm->setHbeatnum(hbeatR);//send hbeat round
    forwardMessage(hbeatm, delay);
}

void fully_connected::sendecho(int echoR, simtime_t delay, string echoW){
    char Msgname[10];
    sprintf(Msgname, "echo to %s sent by : %s ", echoW.c_str(), getName());
    echos *echom = new echos(Msgname, 20);
    echom->setEchonum(echoR);//send echo round
    echom->setSourcename(echoW.c_str());
    forwardMessage(echom, delay);
}



void fully_connected::initialize()// one time or every round?
{
    r = 0;
    delr = 3;
    maxdelay = 1.0;//length of a round
    duration = 5;//time window period
    curtime = simTime();
    count[getIndex()]=0;
    EV<<"index:"<<getIndex()<<"   "<<"name:"<<getName()<<"\n";
    EV<<"sending Heartbeat"<<"\n";
    sendhbeats(r,0);//at round r send hbeat r=0, delay =0
    roundMsg = new cMessage("round over");
    scheduleAt(curtime + maxdelay, roundMsg);//send self round over
}


void fully_connected::handleMessage(cMessage *msg)
{
    if(uniform(0,1)<0.2)
    {
        EV << "Losing message";
        bubble("message lost");
        delete msg;
    }
    else{
        if(msg==roundMsg){// send hbeat or send echo
        EV<<"round over"<<"\n";
        scheduleAt(simTime()+maxdelay, roundMsg);
        r = r+1;
        if(r<duration){
            EV <<"round"<< r<<"\n" ;
            sendhbeats(r,0);
            }
        //delete msg;
        cancelAndDelete(msg);
      }
        if(msg->getKind()==hbeatkind){//receive the hbeats
                EV<<"receive hbeats"<<"\n";
                simtime_t rectime = simTime();
                double p = (rectime - curtime).dbl();
                double m = maxdelay.dbl();
                int t = p/m;
                simtime_t delaytime = curtime + (simtime_t)((t+1)*m) - rectime;
                hbeats *hbeatm = check_and_cast<hbeats *>(msg);
                int hbeatR = hbeatm->getHbeatnum();
                string sourcename = hbeatm->getHbeatname();
                if(r <= (hbeatR + delr)){
                    sendecho(r,delaytime,sourcename);
                    delete msg;
                }
       }
        if(msg->getKind()==20){
            EV<<"get echo";
            echos *echom = check_and_cast<echos *>(msg);
            string name = echom->getSourcename();
            if(name == getName()){
                count[getIndex()]+=1;
                EV<< "count of echos"<<  count <<"\n";
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
        EV << "Forwarding message " << copymsg << " on port out[" << i << "]\n";
        sendDelayed(copymsg, delay, "out", i);
    }
    delete msg;
}


