#ifndef AUTOCROSS_H
#define AUTOCROSS_H


#include "Mission-control.hpp"
#include "cluon-complete.hpp"
#include "opendlv-standard-message-set.hpp"
#include "cfsd-extended-message-set.hpp"
/********
 * 
 */
class Autocross: public MissionControl
{
private:
    /* data */
    long m_start_timestamp;
    float m_steeringReq;
    float m_speedReq;
public:
    Autocross(cluon::OD4Session&, int, int, float, float, bool);
    ~Autocross();
    bool create_data_trigger(); 
    bool remove_data_trigger();
    bool wait();
    bool init();
    bool step();
    bool abort();
};

#endif