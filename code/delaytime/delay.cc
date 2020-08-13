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



using namespace omnetpp;
using namespace std;

class delaysender : public cSimpleModule
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
        map<string, map<int,int>>countmap;//name,map

    public:
        delaysender();
        virtual ~delaysender();
    protected:
        virtual void forwardMessage(cMessage *msg, simtime_t delay);
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;

};

Define_Module(delaysender);

delaysender::delaysender(){
    roundMsg = NULL;
}
delaysender::~delaysender(){
    cancelAndDelete(roundMsg);
}



void delaysender::initialize()// one time or every round?
{
    roundMsg = new cMessage("round over");
    scheduleAt(startime + maxdelay, roundMsg);//send self round over
}


void delaysender::handleMessage(cMessage *msg)
{
    if(msg==roundMsg){
        char Msgname[20];
        sprintf(Msgname, "%s",getName());
        cMessage *m = new cMessage(Msgname);
        forwardMessage(m,1);
    }
}

void delaysender::forwardMessage(cMessage *msg, simtime_t delay)
{

    int n = gateSize("out");
    cMessage *copymsg;
    EV<<"delay"<<delay<<"\n";
    for(int i=0; i<n; i=i+1)
    {
        copymsg = (cMessage *)msg->dup();
        //EV << "Forwarding message " << copymsg << " on port out[" << i << "]\n";
        sendDelayed(copymsg, delay, "out", i);
    }
    delete msg;
}


