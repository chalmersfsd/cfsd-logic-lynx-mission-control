#include "Brake-test.hpp"

BrakeTest::BrakeTest(cluon::OD4Session& od4, int missionID, int freq,bool VERBOSE)
  : MissionControl(od4, missionID, freq, VERBOSE)
{

}

BrakeTest::~BrakeTest()
{
    
}

bool BrakeTest::create_data_trigger(){
    auto dataReading = [](cluon::data::Envelope &&env){
        // opendlv::proxy::SwitchStateReading p = cluon::extractMessage<opendlv::proxy::SwitchStateReading>(std::move(env));
        // // reading the as state from the state machine for 
        // if(env.senderStamp() == 2101){//asState
        //     std::cout << "2101: AsState: " << p.state()<< std::endl;
        // }
        // // reading the mission id
        // if(env.senderStamp() == 1906){ // asMission
        //     std::cout << "[info] \t Mission Selected: " << p.state() << std::endl;
        // }
        
        std::cout << "date trigger in BrakeTest" << std::endl;
    };
    m_od4.dataTrigger(opendlv::proxy::TorqueRequest::ID(), dataReading);

    if(m_VERBOSE){
        std::cout << "Created BrakeTest Data Trigger" << std::endl;
    }
    return true;
}

bool BrakeTest::remove_data_trigger(){
    m_od4.dataTrigger(opendlv::proxy::TorqueRequest::ID(), nullptr);
    if (m_VERBOSE){
        std::cout << "Remove BrakeTest Data Trigger" << std::endl;
    }
    return true;
}

bool BrakeTest::init(){
    if(m_VERBOSE){
        std::cerr <<  "BrakeTest Initialize" << std::endl;
    }
    return true;
}

bool BrakeTest::step(){
    if(m_VERBOSE){
        std::cerr <<  "BrakeTest step" << std::endl;
    }
    return true;
}

bool BrakeTest::abort(){
    if(m_VERBOSE){
        std::cerr <<  "BrakeTest aborted" << std::endl;
    }
    return true;
}

bool BrakeTest::wait(){
    if(m_VERBOSE){
        std::cerr << "BrakeTest Waiting" << std::endl;
    }
    return true;
}