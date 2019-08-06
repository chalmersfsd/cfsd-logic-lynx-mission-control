#ifndef TRACKDRIVE_H
#define TRACKDRIVE_H

#include <array>
#include <mutex>

#include "Mission-control.hpp"
#include "cluon-complete.hpp"
#include "opendlv-standard-message-set.hpp"
#include "cfsd-extended-message-set.hpp"
#include "collector.hpp"

/********
 * 
 */
class Trackdrive: public MissionControl
{
private:
    /* data */
    long m_start_timestamp;
    Collector m_collector;
    pathplannerFlag m_flag;
    int m_laps;
    std::mutex m_gpsMutex;
    bool m_atStart;
    std::array<double, 2> m_startPos;
    std::array<double, 2> m_currentPos;
    double m_gpsDistThres2;
    bool m_isAwayFromStart;
public:
    Trackdrive(cluon::OD4Session&, int, int, double, bool);
    ~Trackdrive();
    bool create_data_trigger(); 
    bool remove_data_trigger();
    bool wait();
    bool init();
    bool step();
    bool abort();
};

#endif