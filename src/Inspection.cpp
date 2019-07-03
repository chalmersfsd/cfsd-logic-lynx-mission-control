#include "Inspection.hpp"

Inspection::Inspection(cluon::OD4Session& od4, int missionID, int freq, int torqueReq, bool VERBOSE)
  : MissionControl(od4, missionID, freq, VERBOSE)
  , m_t{0}
  , m_dt{0}
  , m_start_timestamp{0}
  , m_torqueReq{torqueReq}
{

}

Inspection::~Inspection()
{
    
}

bool Inspection::create_data_trigger()
{
    if(m_VERBOSE){
        std::cout << "Created Inspection Data trigger" << std::endl;
    }
    return true;
}

bool Inspection::remove_data_trigger(){
    if (m_VERBOSE){
        std::cout << "Remove Inspection Data Trigger" << std::endl;
    }
    return true;
}

bool Inspection::init(){
    m_dt = 1.0f / (float)m_freq;
    if(m_VERBOSE){
        std::cout << "Inspection Initialize" << std::endl;
    }
    return true;
}

bool Inspection::step(){
    if (m_missionFinished)
        return true;
    
    cluon::data::TimeStamp ts{cluon::time::now()};
    float s = std::sin(m_t);

    opendlv::proxy::GroundSteeringRequest steer;
    steer.groundSteering(20.0f * s);
    m_od4.send(steer, ts, 2801);
    
    opendlv::cfsdProxy::TorqueRequestDual torque;
    torque.torqueLeft(m_torqueReq);
    torque.torqueRight(m_torqueReq);
    m_od4.send(torque, ts, 2101);
    
    if(m_VERBOSE){
        std::cout << "Inspection step" << std::endl;
        std::cout << "steering request: " << 20.0f * s << std::endl;
    }
    
    // After 25 s to 30 s the mission is finished
    long dt = cluon::time::toMicroseconds(ts) - m_start_timestamp;
    std::cout << "Timer: " << (double)dt / 1e6 << "s" << std::endl;
    if (dt > 25000000) {
        m_missionFinished = true;
        std::cout << "Inspection finished" << std::endl;
    }
    m_t += m_dt;
    return true;
}

bool Inspection::abort(){
    if(m_VERBOSE){
        std::cout << "Inspection aborted" << std::endl;
    }
    return true;
}

bool Inspection::wait(){
    m_start_timestamp = cluon::time::toMicroseconds(cluon::time::now());
    if(m_VERBOSE){
        std::cout << "Inspection Waiting" << std::endl;
    }
    return true;
}