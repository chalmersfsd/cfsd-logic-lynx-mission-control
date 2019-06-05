#ifndef INSPECTION_H
#define INSPECTION_H


#include "Mission-control.hpp"
#include "cluon-complete.hpp"
#include "opendlv-standard-message-set.hpp"
#include "cfsd-extended-message-set.hpp"
#include <cmath>
/********
 * The inspection mission is defined by slowly spinning the drivetrain and actuating the steering
 * system with a sine wave. After 25 s to 30 s the mission is finished and the transition to “AS
 * Finish” must be initialized.
 */
class Inspection: public MissionControl
{
private:
    /* data */
    float m_t;
    float m_dt;
    long m_start_timestamp;
public:
    Inspection(cluon::OD4Session&, int, int, bool);
    ~Inspection();
    bool create_data_trigger(); 
    bool wait();
    bool init();
    bool step();
    bool abort();
};

#endif