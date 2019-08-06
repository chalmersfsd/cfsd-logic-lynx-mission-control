#ifndef MISSIONCONTROLL_H
#define MISSIONCONTROLL_H

#include "collector.hpp"
#include <string>

enum pathplannerFlag {
    AUTO,   // 0
    BLUE,   // 1
    YELLOW, // 2
    PARKING // 3
};

enum MissionState{
    M_WAITING,
    M_RUNNING,
    M_FINISHED,
    M_ERROR,
    M_ABORTED,
    M_STOPPED
};

class MissionControl
{
protected:
    /* data */
    cluon::OD4Session &m_od4;
    Collector m_collector;
public:
    MissionControl(cluon::OD4Session&, int, int, bool);
    virtual ~MissionControl();
    virtual bool create_data_trigger() = 0; 
    virtual bool remove_data_trigger() = 0;
    virtual bool init();
    virtual bool wait() = 0;
    virtual bool step() = 0;
    virtual bool abort() = 0;
    bool finish();
    void startMission(std::string missionName);
    void stopMission();
    void switchWaiting();
    void switchRunning();
    void switchFinished();
    void switchError();
    void switchAborted();
    void switchStopped();
    bool sendMissionState();
    void dockercompose();
    int m_missionID;
    int m_missionState;
    int m_freq;
    bool m_missionFinished;
    std::string m_missionName;
    const bool m_VERBOSE;   
};

#endif