#ifndef BREAKTEST_H
#define BREAKTEST_H


#include "Mission-control.hpp"
#include "cluon-complete.hpp"
#include "opendlv-standard-message-set.hpp"
#include "cfsd-extended-message-set.hpp"
/********
 * The test will be performed in a straight line marked with cones similar to acceleration.
 * During the brake test, the vehicle must accelerate in autonomous mode up to at least 40 km/h
 * within 20 m. From the point where the RES is triggered, the vehicle must come to a safe stop
 * within a maximum distance of 10 m.
 */
class BrakeTest: public MissionControl
{
private:
    /* data */
public:
    BrakeTest(cluon::OD4Session&, int, int, bool);
    ~BrakeTest();
    bool create_data_trigger(); 
    bool remove_data_trigger();
    bool wait();
    bool init();
    bool step();
    bool abort();
};

#endif