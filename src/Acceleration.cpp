#include "Acceleration.hpp"

Acceleration::Acceleration(cluon::OD4Session& od4, int missionID, int freq, bool VERBOSE)
  : MissionControl(od4, missionID, freq, VERBOSE)
{

}

Acceleration::~Acceleration()
{
    
}

bool Acceleration::create_data_trigger(){
    auto SwitchStateReading = [VERBOSE,&stateMachine,&missionID,&missionSelected](cluon::data::Envelope &&env){
        opendlv::proxy::SwitchStateReading p = cluon::extractMessage<opendlv::proxy::SwitchStateReading>(std::move(env));
        
    };
    od4.dataTrigger(opendlv::proxy::SwitchStateReading::ID(), SwitchStateReading);

    if(m_VERBOSE){
        std::cout << "Created Acceleration Data trigger" << std::endl;
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
    if(m_VERBOSE){
        std::cout << "Acceleration step" << std::endl;
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