#include "Trackdrive.hpp"

Trackdrive::Trackdrive(cluon::OD4Session& od4, int missionID, int freq, float speedReq, bool VERBOSE)
  : MissionControl(od4, missionID, freq, VERBOSE)
  , m_start_timestamp{0}
  , m_speedReq{speedReq}
  , m_startPos{}
  , m_currentPos{}
  , m_posMutex{}
  , m_laps{0}
{

}

Trackdrive::~Trackdrive()
{
    
}

bool Trackdrive::create_data_trigger(){
    auto onGeodeticWgs84Reading{[this](cluon::data::Envelope &&envelope) {
        if (envelope.senderStamp() == 112) {
            std::lock_guard<std::mutex> lock(m_posMutex);
            auto msg = cluon::extractMessage<opendlv::proxy::GeodeticWgs84Reading>(std::move(envelope));
            m_currentPos = {msg.latitude(), msg.longitude()};
        }
    }};
    m_od4.dataTrigger(opendlv::proxy::GeodeticWgs84Reading::ID(), onGeodeticWgs84Reading);

    if(m_VERBOSE){
        std::cout << "Created Trackdrive Data Trigger" << std::endl;
    }
    return true;
}

bool Trackdrive::remove_data_trigger(){
    if (m_VERBOSE){
        std::cout << "Remove Trackdrive Data Trigger" << std::endl;
    }
    return true;
}

bool Trackdrive::init(){
    if(m_VERBOSE){
        std::cout << "Trackdrive Initialize" << std::endl;
    }
    return true;
}

bool Trackdrive::step(){
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

    std::lock_guard<std::mutex> lock(m_posMutex);
    std::array<double, 2> distance = wgs84::toCartesian(m_startPos, m_currentPos);
    if (distance[0] * distance[0] + distance[1] * distance[1] < 0.01)
        if (++m_laps == MAX_LAPS)
            m_missionFinished = true;
    
    if(m_VERBOSE){
        std::cout << "Trackdrive step"
                  << "\n  #laps: " << m_laps
                  << "\n  start geolocation:   " << m_startPos[0] << ", " << m_startPos[1]
                  << "\n  current geolocation: " << m_currentPos[0] << ", " << m_currentPos[1]
                  << "\n  cartesian distance:  " << distance[0] << ", " << distance[1] << std::endl;
    }
    
    return true;
}

bool Trackdrive::abort(){
    if(m_VERBOSE){
        std::cout << "Trackdrive aborted" << std::endl;
    }
    return true;
}

bool Trackdrive::wait(){
    m_start_timestamp = cluon::time::toMicroseconds(cluon::time::now());
    std::lock_guard<std::mutex> lock(m_posMutex);
    m_startPos = m_currentPos;
    if(m_VERBOSE){
        std::cout << "Trackdrive Waiting" << std::endl;
    }
    return true;
}
