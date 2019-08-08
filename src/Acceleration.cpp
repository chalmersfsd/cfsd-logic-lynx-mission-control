#include "Acceleration.hpp"

Acceleration::Acceleration(cluon::OD4Session& od4, int missionID, int freq, float speedReq, bool VERBOSE)
  : MissionControl(od4, missionID, freq, VERBOSE)
  , m_flag{pathplannerFlag::PARKING}
  , m_speedReq{speedReq}
  , m_gpsMutex{}
  , m_atStart{true}
  , m_startPos{}
  , m_currentPos{}
  , m_endTimestamp{0}
{

}

Acceleration::~Acceleration()
{
    
}

bool Acceleration::create_data_trigger(){
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
        std::cout << "Created Acceleration Data trigger" << std::endl;
    }
    return true;
}

bool Acceleration::remove_data_trigger(){
    if (m_VERBOSE){
        std::cout << "Remove Acceleration Data Trigger" << std::endl;
    }
    return true;
}

bool Acceleration::init(){
    if(m_VERBOSE){
        std::cout << "Acceleration Initialize" << std::endl;
    }
    return true;
}

bool Acceleration::step(){
    if (m_missionFinished)
        return true;

    m_collector.GetCompleteFrameCFSD19();
    
    // number of [yellow, blue, orange, big orange] cones
    std::array<int, 4> numCones = m_collector.ProcessFrameCFSD19();

    std::lock_guard<std::mutex> lock(m_gpsMutex);
    std::array<double, 2> distance = wgs84::toCartesian(m_startPos, m_currentPos);
    double d = distance[0]*distance[0] + distance[1]*distance[1];
    if (d > 6400 && numCones[2] > 1 && m_speedReq) {
        m_speedReq = -1;
        m_endTimestamp = cluon::time::toMicroseconds(cluon::time::now());
    }

    cluon::data::TimeStamp ts{cluon::time::now()};
    opendlv::proxy::GroundSpeedRequest speed;
    speed.groundSpeed(m_speedReq);
    m_od4.send(speed, ts, 2201);

    opendlv::proxy::SwitchStateRequest flagMsg;
    flagMsg.state(m_flag);
    m_od4.send(flagMsg, cluon::time::now(), 2901);

    double dt = (double) (cluon::time::toMicroseconds(ts) - m_endTimestamp) / 1e6;
    if (m_endTimestamp != 0 && dt > 3) {
        // assume the car should stop within t seconds and then mission finished, here t=3
        m_missionFinished = true;
    }

    if(m_VERBOSE){
        std::cout << "Acceleration step, send speedReq: " << m_speedReq << "m/s" << std::endl;
    }
    return true;
}

bool Acceleration::abort(){
    if(m_VERBOSE){
        std::cout << "Acceleration aborted" << std::endl;
    }
    return true;
}

bool Acceleration::wait(){
    if(m_VERBOSE){
        std::cout << "Acceleration Waiting" << std::endl;
    }
    return true;
}