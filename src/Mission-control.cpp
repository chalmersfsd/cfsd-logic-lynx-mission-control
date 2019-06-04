#include "Mission-control.hpp"

MissionControl::MissionControl(cluon::OD4Session& od4, int missionID, int freq, bool VERBOSE)
  : m_od4{od4}
  , m_missionID{missionID}
  , m_missionState{0}
  , m_freq{freq}
  , m_missionFinished{false}
  , m_VERBOSE{VERBOSE}
{
    if(m_VERBOSE){
        std::cerr <<  "MissionControl Created" << std::endl;
    }

}

MissionControl::~MissionControl()
{
}

bool MissionControl::sendMissionState()
{
    cluon::data::TimeStamp now{cluon::time::now()};
    opendlv::proxy::SwitchStateRequest msg;
    msg.state(m_missionState);
    m_od4.send(msg, now, 1902); //asMissionFinished
    return true;
}

bool MissionControl::init()
{
    return true;
}

bool MissionControl::finish()
{
    m_missionFinished = true;
    return true;
}

void MissionControl::switchWaiting()
{
    m_missionState = MissionState::M_WAITING;
    return;
}
void MissionControl::switchRunning()
{
    m_missionState = MissionState::M_RUNNING;
    return;
}
void MissionControl::switchFinished()
{
    m_missionState = MissionState::M_FINISHED;
    return;
}
void MissionControl::switchError()
{
    m_missionState = MissionState::M_ERROR;
    return;
}
void MissionControl::switchAborted()
{
    m_missionState = MissionState::M_ABORTED;
    return;
}
