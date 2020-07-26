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


using namespace omnetpp;
using namespace std;

class fully_connected : public cSimpleModule
{
    private:
        int r;//round number
        //int nodenum; //the index of the node
        map<string,string> mapSigs;
        hash<string> str_hash;
        simtime_t maxdelay;//the length of round r
        simtime_t curtime;
        cMessage *delayMsg;
        int count;// num of echos including self
        int duration;//maximum round

    public:
        fully_connected();
        virtual ~fully_connected();
    protected:
        virtual void forwardMessage(cMessage *msg);
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;\
        virtual cMessage *generateMsg();
        virtual cMessage *Copymsg(cMessage *msg);
        virtual int checkhbeats(cMessage *msg);
//        virtual void sig(string name);
//        virtual void addsig();
};

Define_Module(fully_connected);

fully_connected::fully_connected(){
    delayMsg = NULL;
}
fully_connected::~fully_connected(){
    cancelAndDelete(delayMsg);
}


cMessage *fully_connected::generateMsg()
{
    curtime = simTime();
    char msgname[20];
    int nodenum = getIndex();
    const char *name = getName();
    //sigs = sig();
    sprintf(msgname, "hb:round-%d node-%d-name%s sigs-%s",r,nodenum,name,mapSigs[name].c_str());
    cMessage *msg = new cMessage(msgname);
    return msg;
}

cMessage *fully_connected::Copymsg(cMessage *msg)
{
    cMessage *copy = (cMessage *)msg->dup();
    return copy;
}


int fully_connected::checkhbeats(cMessage *msg)
{
    return 1;
}

//void fully_connected::sig(string name){//signature for index==?
//    mapSigs[name] = str_hash(name);
//}
//
// void fully_connected::addsig(){
//    const char *name = getName();
//    if(mapSigs.find(name) != mapSigs.end()) {
//        mapSigs[name] = str_hash(name);
//    }
//}




void fully_connected::initialize()// one time or every round?
{
    r = 0;
    maxdelay = 1.0;

    count = 0;
    cMessage *copyhbeats;
    cMessage *hbeats;
    duration = par("Duration");
    delayMsg = new cMessage("round over");
    EV<<"index:   "<<getIndex()<<"name:    "<<getName()<<"\n";

    EV<<"sending Heartbeat\n";
    hbeats = generateMsg();
     //sig(getName());//
    copyhbeats = Copymsg(hbeats);
    forwardMessage(copyhbeats);
    scheduleAt(simTime()+maxdelay, delayMsg);//another one end of period
    r++;

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
//        if (strcmp(getName(),"node1")==0) {
//            EV << "Echo received \n";
//            count++;
//            delete msg;
//        }
//        else {
//            int checkresult = checkhbeats(msg);
//            if(checkresult==1){//real heatbeats
//                //addsig();
//                cMessage *echo = generateMsg();
//                cMessage *copyecho = Copymsg(echo);
//                forwardMessage(copyecho);
//                delete msg;
//            }//else?
        int checkresult = checkhbeats(msg);
        if(checkresult==1){//real heatbeats
                        //addsig();
            //cMessage *echo = generateMsg();
//            char echomsg[20];
//            sprinf(echomsg,"echo-%s",getName());
//            cMessage *echo = new cMessage(echomsg);
            cMessage *echo = generateMsg();
            cMessage *copyecho = Copymsg(echo);
            forwardMessage(copyecho);
            delete msg;
        }
    }

}

void fully_connected::forwardMessage(cMessage *msg)
{

    int n = gateSize("out");
    cMessage *copymsg;

    for(int i=0; i<n; i=i+1)
    {
        copymsg = Copymsg(msg);
        EV << "Forwarding message " << copymsg << " on port out[" << i << "]\n";
        send(copymsg, "out", i);
    }
}


