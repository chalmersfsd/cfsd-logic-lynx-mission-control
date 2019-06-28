#include "Brake-test.hpp"

BrakeTest::BrakeTest(cluon::OD4Session& od4, int missionID, int freq, float speedReq, bool VERBOSE)
  : MissionControl(od4, missionID, freq, VERBOSE)
  , m_speedReq{speedReq}
{

}

BrakeTest::~BrakeTest()
{
    
}

bool BrakeTest::create_data_trigger(){
    if(m_VERBOSE){
        std::cout << "Created BrakeTest Data Trigger" << std::endl;
    }
    return true;
}

bool BrakeTest::remove_data_trigger(){
    if (m_VERBOSE){
        std::cout << "Remove BrakeTest Data Trigger" << std::endl;
    }
    return true;
}

bool BrakeTest::init(){
    if(m_VERBOSE){
        std::cout << "BrakeTest Initialize" << std::endl;
    }
    return true;
}

bool BrakeTest::step(){
    if (m_missionFinished)
        return true;

    cluon::data::TimeStamp ts{cluon::time::now()};
    opendlv::proxy::GroundSpeedRequest speed;
    speed.groundSpeed(m_speedReq);
    m_od4.send(speed, ts, 2201);

    if(m_VERBOSE){
        std::cout << "BrakeTest step, send speedReq: " << m_speedReq << "m/s" << std::endl;
    }
    return true;
}

bool BrakeTest::abort(){
    if(m_VERBOSE){
        std::cout << "BrakeTest aborted" << std::endl;
    }
    return true;
}

bool BrakeTest::wait(){
    if(m_VERBOSE){
        std::cout << "BrakeTest Waiting" << std::endl;
    }
    return true;
}