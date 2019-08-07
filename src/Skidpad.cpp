#include "Skidpad.hpp"

Skidpad::Skidpad(cluon::OD4Session& od4, int missionID, int freq, double gpsDistThres, bool VERBOSE)
  : MissionControl(od4, missionID, freq, VERBOSE)
  // , m_startTimestamp{0}
  , m_flag{pathplannerFlag::AUTO}
  , m_laps{0}
  , m_gpsMutex{}
  , m_atStart{true}
  , m_reachIntersection{false}
  , m_startPos{}
  , m_currentPos{}
  , m_gpsDistThres2{gpsDistThres*gpsDistThres}
  , m_isAwayFromStart{false}
{
    init();
}

Skidpad::~Skidpad()
{
    
}

bool Skidpad::create_data_trigger() {
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
        std::cout << "Created Skidpad Data Trigger" << std::endl;
    }
    return true;
}

bool Skidpad::remove_data_trigger(){
    if (m_VERBOSE){
        std::cout << "Remove Skidpad Data Trigger" << std::endl;
    }
    return true;
}

bool Skidpad::init(){
    if(m_VERBOSE){
        std::cout << "Skidpad Initialize" << std::endl;
    }
    create_data_trigger();
    return true;
}

bool Skidpad::step(){
    if (m_missionFinished)
        return true;

    // cluon::data::TimeStamp ts{cluon::time::now()};
    // opendlv::proxy::GroundSpeedRequest speed;
    // speed.groundSpeed(m_speedReq);
    // m_od4.send(speed, ts, 2201);

    // double dt = (double) (cluon::time::toMicroseconds(ts) - m_startTimestamp) / 1e6;
    // int tq = (int) (20 * dt);
    // if (tq > 100)
    //     tq = 100;
    // opendlv::cfsdProxy::TorqueRequestDual torque;
    // torque.torqueLeft(tq);
    // torque.torqueRight(tq);
    // m_od4.send(torque, ts, 2101);

    // opendlv::proxy::GroundSteeringRequest steer;
    // steer.groundSteering(m_steeringReq);
    // m_od4.send(steer, ts, 2801);
    
    m_collector.GetCompleteFrameCFSD19();

    // number of [yellow, blue, orange, big orange] cones
    std::array<int, 4> numCones = m_collector.ProcessFrameCFSD19();

    std::lock_guard<std::mutex> lock(m_gpsMutex);
    std::array<double, 2> distance = wgs84::toCartesian(m_startPos, m_currentPos);
    double d = distance[0]*distance[0] + distance[1]*distance[1];
    if (m_reachIntersection) {
        if (d < m_gpsDistThres2) {
            if (m_isAwayFromStart && numCones[3] > 1) { // at least see 2 big orange cones
                m_laps++;
                m_isAwayFromStart = false;
            }
        }
        else {
            m_isAwayFromStart = true;
        }

        switch (m_laps) {
            case 0:
            case 1:
                m_flag = pathplannerFlag::YELLOW;
                break;
            case 2:
            case 3:
                m_flag = pathplannerFlag::BLUE;
                break;
            case 4:
                m_flag = pathplannerFlag::PARKING;
                if (d > 64 && d < 100 && numCones[2] > 2) // at least see 3 orange cones
                    m_missionFinished = true;
        }
    }
    else if (d > 14.5*14.5 && d < 15.5*15.5 && numCones[3] > 1) { // at least see 2 big orange cones
        m_startPos = m_currentPos;
        m_reachIntersection = true;
    }

    opendlv::proxy::SwitchStateRequest flagMsg;
    flagMsg.state(m_flag);
    m_od4.send(flagMsg, cluon::time::now(), 2901);

    if(m_VERBOSE)
        std::cout << "Skidpad step: " << m_laps << " laps" << std::endl;
    
    return true;
}

bool Skidpad::abort(){
    if(m_VERBOSE){
        std::cout << "Skidpad aborted" << std::endl;
    }
    return true;
}

bool Skidpad::wait(){
    // m_startTimestamp = cluon::time::toMicroseconds(cluon::time::now());
    if(m_VERBOSE){
        std::cout << "Skidpad Waiting" << std::endl;
    }
    return true;
}
