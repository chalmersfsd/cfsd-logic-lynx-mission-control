#include "Inspection.hpp"

Inspection::Inspection(cluon::OD4Session& od4, int missionID, int freq, bool VERBOSE)
  : MissionControl(od4, missionID, freq, VERBOSE)
  , m_time{0}
  , m_dt{0}
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

bool Inspection::init(){
    m_dt = 1.0f / (float)m_freq;
    if(m_VERBOSE){
        std::cout << "Inspection Initialize" << std::endl;
    }
    return true;
}

bool Inspection::step(){
    cluon::data::TimeStamp ts{cluon::time::now()};
    opendlv::proxy::GroundSteeringRequest steer;
    opendlv::proxy::GroundSpeedRequest speed;
    float s = std::sin(m_time);
    steer.groundSteering(1.0f * s);
    speed.groundSpeed(2.0f * s);
    m_od4.send(steer, ts, 2801);
    m_od4.send(speed, ts, 2201);
    if(m_VERBOSE){
        std::cout << "Inspection step" << std::endl;
        std::cout << "steering request: " << 1.0f * s << std::endl;
        std::cout << "speed request: " << 2.0f * s << std::endl;
    }
    if (m_time > 25) {
        m_missionFinished = true;
        mission -> switchFinished();
    }
    m_time += m_dt;
    return true;
}

bool Inspection::abort(){
    if(m_VERBOSE){
        std::cout << "Inspection aborted" << std::endl;
    }
    return true;
}

bool Inspection::wait(){
    if(m_VERBOSE){
        std::cout << "Inspection Waiting" << std::endl;
    }
    return true;
}