//
// Generated file, do not edit! Created by nedtool 5.6 from hbeats.msg.
//

#ifndef __HBEATS_M_H
#define __HBEATS_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0506
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



/**
 * Class generated from <tt>hbeats.msg:1</tt> by nedtool.
 * <pre>
 * message hbeats
 * {
 *     int hbeatnum;
 *     string hbeatname;
 *     string val;
 * }
 * </pre>
 */
class hbeats : public ::omnetpp::cMessage
{
  protected:
    int hbeatnum;
    ::omnetpp::opp_string hbeatname;
    ::omnetpp::opp_string val;

  private:
    void copy(const hbeats& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const hbeats&);

  public:
    hbeats(const char *name=nullptr, short kind=0);
    hbeats(const hbeats& other);
    virtual ~hbeats();
    hbeats& operator=(const hbeats& other);
    virtual hbeats *dup() const override {return new hbeats(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getHbeatnum() const;
    virtual void setHbeatnum(int hbeatnum);
    virtual const char * getHbeatname() const;
    virtual void setHbeatname(const char * hbeatname);
    virtual const char * getVal() const;
    virtual void setVal(const char * val);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const hbeats& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, hbeats& obj) {obj.parsimUnpack(b);}


#endif // ifndef __HBEATS_M_H

