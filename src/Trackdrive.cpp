#include "Trackdrive.hpp"

Trackdrive::Trackdrive(cluon::OD4Session& od4, int missionID, int freq, double gpsDistThres, bool VERBOSE)
  : MissionControl(od4, missionID, freq, VERBOSE)
  , m_start_timestamp{0}
  , m_collector(VERBOSE)
  , m_flag{pathplannerFlag::AUTO}
  , m_laps{0}
  , m_gpsMutex{}
  , m_atStart{true}
  , m_startPos{}
  , m_currentPos{}
  , m_gpsDistThres2{gpsDistThres*gpsDistThres}
  , m_isAwayFromStart{false}
{

}

Trackdrive::~Trackdrive()
{
    
}

bool Trackdrive::create_data_trigger(){
    auto onObjectFrameStart{[this](cluon::data::Envelope &&envelope) {
        m_collector.getObjectFrameStart(envelope);
    }};

    auto onObjectFrameEnd{[this](cluon::data::Envelope &&envelope) {
        m_collector.getObjectFrameEnd(envelope);
    }};

    auto onObjectType{[this](cluon::data::Envelope &&envelope) {
        m_collector.getObjectType(envelope);
    }};

    auto onObjectPosition{[this](cluon::data::Envelope &&envelope) {
        m_collector.getObjectPosition(envelope);
    }};

    auto onEquilibrioception{[this](cluon::data::Envelope &&envelope) {
        m_collector.getEquilibrioception(envelope);
    }};

    auto onGeodeticWgs84Reading{[this](cluon::data::Envelope &&envelope) {
        std::lock_guard<std::mutex> lock(m_gpsMutex);
        auto msg = cluon::extractMessage<opendlv::proxy::GeodeticWgs84Reading>(std::move(envelope));
        if (m_atStart) {
            m_startPos = {msg.latitude(), msg.longitude()};
            m_atStart = false;
        }
        m_currentPos = {msg.latitude(), msg.longitude()};
    }};

    m_od4.dataTrigger(opendlv::logic::perception::ObjectFrameStart::ID(), onObjectFrameStart);
    m_od4.dataTrigger(opendlv::logic::perception::ObjectFrameEnd::ID(), onObjectFrameEnd);
    m_od4.dataTrigger(opendlv::logic::perception::ObjectType::ID(), onObjectType);
    m_od4.dataTrigger(opendlv::logic::perception::ObjectPosition::ID(), onObjectPosition);
    m_od4.dataTrigger(opendlv::logic::sensation::Equilibrioception::ID(), onEquilibrioception);
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
    // opendlv::proxy::GroundSpeedRequest speed;
    // speed.groundSpeed(m_speedReq);
    // m_od4.send(speed, ts, 2201);

    // double dt = (double) (cluon::time::toMicroseconds(ts) - m_start_timestamp) / 1e6;
    // int tq = (int) (20 * dt);
    // if (tq > 100)
    //     tq = 100;
    // opendlv::cfsdProxy::TorqueRequestDual torque;
    // torque.torqueLeft(tq);
    // torque.torqueRight(tq);
    // m_od4.send(torque, ts, 2101);

    m_collector.GetCompleteFrameCFSD19();
    int numOrangeCones = m_collector.ProcessFrameCFSD19();

    std::lock_guard<std::mutex> lock(m_gpsMutex);
    std::array<double, 2> distance = wgs84::toCartesian(m_startPos, m_currentPos);
    double d = distance[0]*distance[0] + distance[1]*distance[1];
    if (d < m_gpsDistThres2) {
        if (m_isAwayFromStart && numOrangeCones > 1) {
            m_laps++;
            m_isAwayFromStart = false;
        }
    }
    else {
        m_isAwayFromStart = true;
    }

    opendlv::proxy::SwitchStateRequest flagMsg;
    flagMsg.state(m_flag);
    m_od4.send(flagMsg, cluon::time::now(), 2901);

    m_missionFinished = (m_laps == 10);

    if (m_VERBOSE)
        std::cout << "Trackdrive step: " << m_laps << " laps" << std::endl;
    
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
    if(m_VERBOSE){
        std::cout << "Trackdrive Waiting" << std::endl;
    }
    return true;
}
