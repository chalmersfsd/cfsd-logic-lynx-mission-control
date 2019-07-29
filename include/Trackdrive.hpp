#ifndef TRACKDRIVE_H
#define TRACKDRIVE_H

#include <array>
#include <mutex>

#include "Mission-control.hpp"
#include "cluon-complete.hpp"
#include "opendlv-standard-message-set.hpp"
#include "cfsd-extended-message-set.hpp"
#include "WGS84toCartesian.hpp"

#define MAX_LAPS 1
/********
 * 
 */
class Trackdrive: public MissionControl
{
private:
    /* data */
    long m_start_timestamp;
    float m_speedReq;
    std::array<double, 2> m_startPos;
    std::array<double, 2> m_currentPos;
    std::mutex m_posMutex;
    int m_laps;
public:
    Trackdrive(cluon::OD4Session&, int, int, float, bool);
    ~Trackdrive();
    bool create_data_trigger(); 
    bool remove_data_trigger();
    bool wait();
    bool init();
    bool step();
    bool abort();
};

#endif