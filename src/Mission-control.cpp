#include "Mission-control.hpp"

MissionControl::MissionControl(cluon::OD4Session& od4, int missionID, int freq, bool VERBOSE)
  : m_od4{od4}
  , m_collector(VERBOSE)
  , m_missionID{missionID}
  , m_missionState{0}
  , m_freq{freq}
  , m_missionFinished{false}
  , m_missionName {""}
  , m_VERBOSE{VERBOSE}
{
    if(m_VERBOSE){
        std::cout << "MissionControl Created" << std::endl;
    }

}

MissionControl::~MissionControl()
{
}

bool MissionControl::sendMissionState()
{
    // cluon::data::TimeStamp now{cluon::time::now()};
    // opendlv::proxy::SwitchStateRequest msg;
    // msg.state(m_missionState);
    // m_od4.send(msg, now, 1902);
    return true;
}

// first ssh in to the host
void MissionControl::startMission(std::string missionName){
    this -> m_missionName = missionName;

    std::string systemCommand = "sshpass -p cfsd ssh -o StrictHostKeyChecking=no cfsd@172.17.0.1 ";

    // cd ~/missionName/script
    // sh missionName-up.sh

    // inspection.sh
    std::string startMissionCommand = "";
    startMissionCommand += systemCommand;
    startMissionCommand += " \" cd ~/script/";
    startMissionCommand += missionName;
    startMissionCommand += "&& sh ";
    startMissionCommand += missionName;
    startMissionCommand += "-up";
    startMissionCommand += ".sh \" ";

    std::cout << "Up command: " << startMissionCommand << std::endl;


    std::cout << "Starting Mission... " << system(startMissionCommand.c_str()) << " done." << std::endl;

    cluon::data::TimeStamp sampleTime = cluon::time::now();
}

void MissionControl::stopMission(){
    std::string systemCommand = "sshpass -p cfsd ssh -o StrictHostKeyChecking=no cfsd@172.17.0.1 ";

    // cd ~/missionName/script
    // sh missionName-down.sh

    std::string startMissionCommand = "";
    startMissionCommand += systemCommand;
    startMissionCommand += " \" cd ~/script/";
    startMissionCommand += this -> m_missionName;
    startMissionCommand += "&& sh ";
    startMissionCommand += this -> m_missionName;
    startMissionCommand += "-down";
    startMissionCommand += ".sh \" ";

    std::cout << "Down command: " << startMissionCommand << std::endl;


    std::cout << "Stop Mission... " << system(startMissionCommand.c_str()) << " done." << std::endl;

    cluon::data::TimeStamp sampleTime = cluon::time::now();
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
    cluon::data::TimeStamp now{cluon::time::now()};
    opendlv::proxy::SwitchStateRequest msg;
    msg.state(m_missionState);
    m_od4.send(msg, now, 2102); //finishedSignal
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

void MissionControl::switchStopped()
{
    m_missionState = MissionState::M_STOPPED;
    return;
}
