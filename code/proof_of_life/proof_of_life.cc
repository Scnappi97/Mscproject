/*
 * proof_of_life.cc
 *
 *  Created on: 2020Äê7ÔÂ8ÈÕ
 *      Author: 13671
 */

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;
using namespace std;

class fully_connected : public cSimpleModule
{
    private:
        int r;//round number
        int nodenum; //the index of the node
        char *sigs;
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
        virtual char *sig();
        virtual char *addsig();
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
    nodenum = getIndex();
    //sigs = sig();
    sprintf(msgname, "hb:round-%d node-%d sigs-%s",r,nodenum,sigs);
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

char *fully_connected::sig(){//signature for index==?
    return "sig";
}

char *fully_connected::addsig(){
    return "addedsigs";
}




void fully_connected::initialize()// one time or every round?
{
    r = 0;
    maxdelay = 1.0;
    count = 0;
    cMessage *copyhbeats;
    cMessage *hbeats;
    duration = par("Duration");
    delayMsg = new cMessage("round over");
    if(getIndex()==0){

            sigs = sig();
            EV<<"sending Heartbeat\n";
            hbeats = generateMsg();
            copyhbeats = Copymsg(hbeats);
            forwardMessage(copyhbeats);
            scheduleAt(simTime()+maxdelay, delayMsg);
            r++;

    }
}


void fully_connected::handleMessage(cMessage *msg)
{
    if(uniform(0,1)<0.2 )
    {
        EV << "Losing message";
        bubble("message lost");
        delete msg;
    }
    else{
        if (getIndex() == 0) {
            EV << "Echo received \n";
            count++;
            delete msg;
        }
        else {
            int checkresult = checkhbeats(msg);
            if(checkresult==1){//real heatbeats
                sigs = addsig();
                cMessage *echo = generateMsg();
                cMessage *copyecho = Copymsg(echo);
                forwardMessage(copyecho);
                delete msg;
            }//else?
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


