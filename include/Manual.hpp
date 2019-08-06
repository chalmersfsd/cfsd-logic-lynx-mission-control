#ifndef MANUAL_H
#define MANUAL_H


#include "Mission-control.hpp"
#include "cluon-complete.hpp"
#include "opendlv-standard-message-set.hpp"
#include "cfsd-extended-message-set.hpp"
/********
 * 
 */
class Manual: public MissionControl
{
private:
    /* data */
    long m_startTimestamp;
    float m_steeringReq;
    float m_speedReq;
public:
    Manual(cluon::OD4Session&, int, int, float, float, bool);
    ~Manual();
    bool create_data_trigger(); 
    bool remove_data_trigger();
    bool wait();
    bool init();
    bool step();
    bool abort();
};

#endif