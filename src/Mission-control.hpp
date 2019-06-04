#ifndef MISSIONCONTROLL_H
#define MISSIONCONTROLL_H

#include "cluon-complete.hpp"
#include "opendlv-standard-message-set.hpp"
#include "cfsd-extended-message-set.hpp"

enum MissionState{
    M_WAITING,
    M_RUNNING,
    M_FINISHED,
    M_ERROR,
    M_ABORTED
};

class MissionControl
{
protected:
    /* data */
    cluon::OD4Session &m_od4;
public:
    MissionControl(cluon::OD4Session&, int, int, bool);
    virtual ~MissionControl();
    virtual bool create_data_trigger() = 0; 
    virtual bool init();
    virtual bool wait() = 0;
    virtual bool step() = 0;
    virtual bool abort() = 0;
    bool finish();

    void switchWaiting();
    void switchRunning();
    void switchFinished();
    void switchError();
    void switchAborted();
    bool sendMissionState();
    void dockercompose();
    int m_missionID;
    int m_missionState;
    int m_freq;
    bool m_missionFinished;
    const bool m_VERBOSE;
};

#endif