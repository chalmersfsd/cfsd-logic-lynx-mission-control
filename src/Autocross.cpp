#include "Autocross.hpp"

Autocross::Autocross(cluon::OD4Session& od4, int missionID, int freq, float steeringReq, float speedReq, bool VERBOSE)
  : MissionControl(od4, missionID, freq, VERBOSE)
  , m_start_timestamp{0}
  , m_steeringReq{steeringReq}
  , m_speedReq{speedReq}
{

}

Autocross::~Autocross()
{
    
}

bool Autocross::create_data_trigger(){
    if(m_VERBOSE){
        std::cout << "Created Autocross Data Trigger" << std::endl;
    }
    return true;
}

bool Autocross::remove_data_trigger(){
    if (m_VERBOSE){
        std::cout << "Remove Autocross Data Trigger" << std::endl;
    }
    return true;
}

bool Autocross::init(){
    if(m_VERBOSE){
        std::cout << "Autocross Initialize" << std::endl;
    }
    return true;
}

bool Autocross::step(){
    if (m_missionFinished)
        return true;

    cluon::data::TimeStamp ts{cluon::time::now()};
    //opendlv::proxy::GroundSpeedRequest speed;
    //speed.groundSpeed(m_speedReq);
    //m_od4.send(speed, ts, 2201);

    double dt = (double) (cluon::time::toMicroseconds(ts) - m_start_timestamp) / 1e6;
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
        std::cout << "Autocross step" << std::endl
        //          << "  steeringReq: " << m_steeringReq << std::endl
                  << "  speedReq: " << m_speedReq << "m/s" << std::endl;
    }
    
    return true;
}

bool Autocross::abort(){
    if(m_VERBOSE){
        std::cout << "Autocross aborted" << std::endl;
    }
    return true;
}

bool Autocross::wait(){
    m_start_timestamp = cluon::time::toMicroseconds(cluon::time::now());
    if(m_VERBOSE){
        std::cout << "Autocross Waiting" << std::endl;
    }
    return true;
}
