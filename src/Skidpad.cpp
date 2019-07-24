#include "Skidpad.hpp"

Skidpad::Skidpad(cluon::OD4Session& od4, int missionID, int freq, bool VERBOSE)
  : MissionControl(od4, missionID, freq, VERBOSE)
  , m_start_timestamp{0}
  , m_collector(VERBOSE)
{
    init();
}

Skidpad::~Skidpad()
{
    
}

bool Skidpad::create_data_trigger() {
    auto onObjectFrameStart{[this](cluon::data::Envelope &&envelope) {
        // TODO: for now assume senderStamp 0 for perception (add SLAM later)
        if (envelope.senderStamp() == 0) {
            m_collector.getObjectFrameStart(envelope);
        }
    }};

    auto onObjectFrameEnd{[this](cluon::data::Envelope &&envelope) {
        // TODO: for now assume senderStamp 0 for perception (add SLAM later)
        if (envelope.senderStamp() == 0) {
            m_collector.getObjectFrameEnd(envelope);
        }
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
    
    auto atFrequency{[this]() -> bool {
        m_collector.GetCompleteFrameCFSD19();
        m_collector.ProcessFrameCFSD19();

        // opendlv::logic::action::LocalPath localPath;
        // localPath.length(length);
        // localPath.data(data);
        // m_od4.send(localPath, ts, localPathSenderId);

        return true;
    }};


    m_od4.dataTrigger(opendlv::logic::perception::ObjectFrameStart::ID(), onObjectFrameStart);
    m_od4.dataTrigger(opendlv::logic::perception::ObjectFrameEnd::ID(), onObjectFrameEnd);
    m_od4.dataTrigger(opendlv::logic::perception::ObjectType::ID(), onObjectType);
    m_od4.dataTrigger(opendlv::logic::perception::ObjectPosition::ID(), onObjectPosition);
    m_od4.dataTrigger(opendlv::logic::sensation::Equilibrioception::ID(), onEquilibrioception);
    m_od4.timeTrigger(m_freq, atFrequency);

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
        std::cout << "Skidpad step" << std::endl;
    }
    
    return true;
}

bool Skidpad::abort(){
    if(m_VERBOSE){
        std::cout << "Skidpad aborted" << std::endl;
    }
    return true;
}

bool Skidpad::wait(){
    m_start_timestamp = cluon::time::toMicroseconds(cluon::time::now());
    if(m_VERBOSE){
        std::cout << "Skidpad Waiting" << std::endl;
    }
    return true;
}
