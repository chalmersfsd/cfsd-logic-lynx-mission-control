#ifndef SKIDPAD_H
#define SKIDPAD_H


#include "Mission-control.hpp"
#include "cluon-complete.hpp"
#include "opendlv-standard-message-set.hpp"
#include "cfsd-extended-message-set.hpp"

/********
 * 
 */

class Skidpad: public MissionControl
{
private:
    /* data */
    // long m_startTimestamp;
    pathplannerFlag m_flag;
    int m_laps;
    std::mutex m_gpsMutex;
    bool m_atStart;
    bool m_reachIntersection;
    std::array<double, 2> m_startPos;
    std::array<double, 2> m_currentPos;
    double m_gpsDistThres2;
    bool m_isAwayFromStart;
public:
    Skidpad(cluon::OD4Session&, int, int, double, bool);
    ~Skidpad();
    bool create_data_trigger(); 
    bool remove_data_trigger();
    bool wait();
    bool init();
    bool step();
    bool abort();
};

#endif // SKIDPAD_H