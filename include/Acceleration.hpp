#ifndef ACCELERATION_H
#define ACCELERATION_H


#include "Mission-control.hpp"
#include "cluon-complete.hpp"
#include "opendlv-standard-message-set.hpp"
#include "cfsd-extended-message-set.hpp"

/********
 * The acceleration track is a straight line with a length of 75 m from starting line to finish line.
 * The track is at least 3 m wide. Cones are placed along the track at intervals of about 5 m.
 * Cone locations are not marked on the pavement.
 * 
 * Staging - The foremost part of the vehicle is staged at 0.30 m behind the starting line.
 * Vehicles will accelerate from a standing start.
 * 
 * Starting - A go-signal from RES is used to indicate the approval to begin, timing starts only
 * after the vehicle crosses the starting line and stops after it crosses the finish line.
 * 
 * After the finish line, the vehicle must come to a full stop within 100 m inside the marked exit
 * lane and enter the finish-state described in DV 2.3.
 */
class Acceleration: public MissionControl
{
private:
    /* data */
    float m_speedReq;
    std::mutex m_gpsMutex;
    bool m_atStart;
    std::array<double, 2> m_startPos;
    std::array<double, 2> m_currentPos;
    long m_endTimestamp;
public:
    Acceleration(cluon::OD4Session&, int, int, float, bool);
    ~Acceleration();
    bool create_data_trigger(); 
    bool remove_data_trigger();
    bool wait();
    bool init();
    bool step();
    bool abort();
};

#endif