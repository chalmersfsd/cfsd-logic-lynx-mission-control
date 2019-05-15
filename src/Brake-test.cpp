#include "Brake-test.hpp"

BrakeTest::BrakeTest(cluon::OD4Session& od4, int missionID, int freq,bool VERBOSE)
  : MissionControl(od4, missionID, freq, VERBOSE)
{

}

BrakeTest::~BrakeTest()
{
    
}

bool BrakeTest::create_data_trigger()
{
    if(m_VERBOSE){
        std::cerr <<  "Created BrakeTest Data trigger" << std::endl;
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