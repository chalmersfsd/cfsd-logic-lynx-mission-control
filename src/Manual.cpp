#include "Manual.hpp"

Manual::Manual(cluon::OD4Session& od4, int missionID, int freq, float steeringReq, float speedReq, bool VERBOSE)
  : MissionControl(od4, missionID, freq, VERBOSE)
  , m_startTimestamp{0}
  , m_steeringReq{steeringReq}
  , m_speedReq{speedReq}
{

}

Manual::~Manual()
{
    
}

bool Manual::create_data_trigger(){
    if(m_VERBOSE){
        std::cout << "Created Manual Data Trigger" << std::endl;
    }
    return true;
}

bool Manual::remove_data_trigger(){
    if (m_VERBOSE){
        std::cout << "Remove Manual Data Trigger" << std::endl;
    }
    return true;
}

bool Manual::init(){
    if(m_VERBOSE){
        std::cout << "Manual Initialize" << std::endl;
    }
    return true;
}

bool Manual::step(){
    if (m_missionFinished)
        return true;

    cluon::data::TimeStamp ts{cluon::time::now()};
    //opendlv::proxy::GroundSpeedRequest speed;
    //speed.groundSpeed(m_speedReq);
    //m_od4.send(speed, ts, 2201);

    double dt = (double) (cluon::time::toMicroseconds(ts) - m_startTimestamp) / 1e6;
    int tq = (int) (20 * dt);
    if (tq > 100)
        tq = 100;
    opendlv::cfsdProxy::TorqueRequestDual torque;
    torque.torqueLeft(tq);
    torque.torqueRight(tq);
    m_od4.send(torque, ts, 2101);

    // opendlv::proxy::GroundSteeringRequest steer;
    // steer.groundSteering(m_steeringReq);
    // m_od4.send(steer, ts, 2801);
    
    if(m_VERBOSE){
        std::cout << "Manual step" << std::endl
        //          << "  steeringReq: " << m_steeringReq << std::endl
                  << "  speedReq: " << m_speedReq << "m/s" << std::endl;
    }
    
    return true;
}

bool Manual::abort(){
    if(m_VERBOSE){
        std::cout << "Manual aborted" << std::endl;
    }
    return true;
}

bool Manual::wait(){
    m_startTimestamp = cluon::time::toMicroseconds(cluon::time::now());
    if(m_VERBOSE){
        std::cout << "Manual Waiting" << std::endl;
    }
    return true;
}
