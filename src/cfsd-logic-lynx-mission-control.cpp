/*
 * Copyright (C) 2018  Naichen Wang
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "cluon-complete.hpp"
#include "opendlv-standard-message-set.hpp"
//#include "cfsd-extended-message-set.hpp"

#include "Mission-control.hpp"
#include "Brake-test.hpp"
#include "Inspection.hpp"
#include "cfsd-logic-lynx-mission-control.hpp"
#include <cstdint>
#include <iostream>

int32_t main(int32_t argc, char **argv) {
    auto commandlineArguments = cluon::getCommandlineArguments(argc, argv);
    if (0 == commandlineArguments.count("cid")) {
        std::cerr << argv[0] << "Mission control." << std::endl;
        std::cerr << "Usage:   " << argv[0] << " --cid=<OD4 session> mission=<Mission No> [--verbose]" << std::endl;
        std::cerr << "         --cid:    CID of the OD4Session to send and receive messages" << std::endl;
        std::cerr << "         --mission:index of the Mission" << std::endl;
        std::cerr << "Example: " << argv[0] << " --cid=131 --mission=0 --frequency=66 --torqueRequest=120 --verbose" << std::endl;
    }
    else {
        const bool VERBOSE{commandlineArguments.count("verbose") != 0};
        uint16_t cid = static_cast<uint16_t>(std::stoi(commandlineArguments["cid"]));
        uint16_t missionID = 0;
        bool missionSelected = false;
        MissionControl* mission = NULL;
        //Get the mission ID from command line. if it is not setted, it will wait for the state machine send a request
        if(commandlineArguments.count("mission") != 0){
            missionID = static_cast<uint16_t>(std::stoi(commandlineArguments["mission"]));
            if(VERBOSE){
                std::cerr << "[info] \t Mission selected: "<< missionID << std::endl;
            }
        }else{
            std::cerr << "[info] \t Mission not selected! waiting for stateMachine" << std::endl;
        }
        // if the frequency is not set, then it runs at 60Hz as default
        uint16_t frequency = 60;

        if (commandlineArguments.count("frequency") != 0)
        {
            frequency = static_cast<uint16_t>(std::stoi(commandlineArguments["frequency"]));
        }
        // Create sessions    
        cluon::OD4Session od4{cid};
        if(VERBOSE){
            std::cerr << "Start conversation at Opendlv session cid: " << cid << std::endl;
        }

        // always read the as state 
        int stateMachine = 0;
        auto SwitchStateReading = [VERBOSE,&stateMachine,&missionID,&missionSelected](cluon::data::Envelope &&env){
            opendlv::proxy::SwitchStateReading p = cluon::extractMessage<opendlv::proxy::SwitchStateReading>(std::move(env));
            // reading the as state from the state machine
            if (env.senderStamp() == 2101){ // asState
                stateMachine = p.state();
                if (VERBOSE){
                    //std::cout << "2101: AsState: " << p.state()<< std::endl;
                }
            }
            // reading the mission id
            else if (env.senderStamp() == 1906){ // asMission
                missionID = p.state();
                if (VERBOSE && !missionSelected){
                    std::cout << "[info] \t Mission Selected: " << missionID << std::endl;
                }
            }
        };
        od4.dataTrigger(opendlv::proxy::SwitchStateReading::ID(), SwitchStateReading);

        uint16_t torqueRequest = static_cast<uint16_t>(std::stoi(commandlineArguments["torqueRequest"]));

        auto missionStep = [VERBOSE,&od4,&mission,&stateMachine,&missionID,&missionSelected,frequency,torqueRequest]() -> bool{
            bool res = true;
            // initialization stage: if no mission is selected yet, and mission is none, and asState is ready
            if (missionSelected == false && missionID > 0 && stateMachine == asState::AS_READY){
                // create mission
                switch (missionID) {
                    case asMission::AMI_BRAKETEST:
                        mission = new BrakeTest(od4, missionID, frequency, VERBOSE);
                        mission -> startMission("braketest");
                        break;
                    case asMission::AMI_INSPECTION:
                        
                        mission = new Inspection(od4, missionID, frequency, torqueRequest, VERBOSE);
                        mission -> startMission("inspection");
                        break;
                    case asMission::AMI_ACCELERATION:
                        // mission = new Acceleration(od4, missionID, frequency, VERBOSE);
                        // mission -> startMission("acceleration");
                        std::cerr <<  "[Error] \t Mission ID " << missionID <<" has not implemented yet." << std::endl;
                        break;
                    case asMission::AMI_SKIDPAD:
                        std::cerr <<  "[Error] \t Mission ID " << missionID <<" has not implemented yet." << std::endl;
                        //todo
                        break;
                    case asMission::AMI_TRACKDRIVE:
                        //todo
                        std::cerr <<  "[Error] \t Mission ID " << missionID <<" has not implemented yet." << std::endl;
                        break;
                    case asMission::AMI_AUTOCROSS:
                        //todo
                        std::cerr <<  "[Error] \t Mission ID " << missionID <<" has not implemented yet." << std::endl;
                        break;
                    case asMission::AMI_MANUAL:
                        //todo
                        std::cerr <<  "[Error] \t Mission ID " << missionID <<" has not implemented yet." << std::endl;
                        break;
                    case asMission::AMI_TEST:
                        //todo
                        std::cerr <<  "[Error] \t Mission ID " << missionID <<" has not implemented yet." << std::endl;
                        break;
                    default:
                        std::cerr <<  "[Error] \t Mission ID " << missionID <<" is wrong or has not implemented yet." << std::endl;
                        return false ;
                }
                //initialize if needed
                mission->init();

                // The data trigger should be handeled by missioncontrol itself
                mission->create_data_trigger();
                missionSelected = true;
            }

            // will start the mission control after mission is selected.
            if (missionID > 0 && missionSelected == true){
            // Before the mission start (AS_Ready), mission control should wait (or do something)
                switch (stateMachine) {
                    case asState::AS_READY:
                        res = mission -> wait();
                        mission -> switchWaiting();
                        break;
                    case asState::AS_DRIVING: // when mission start, it run steps
                        res = mission -> step();
                        if (!res){//if the step failed
                            mission->switchError();
                        }else{//working fine
                            if (mission -> m_missionFinished){// if mission finished
                                mission -> switchFinished();
                            }else{// keep running
                                mission -> switchRunning();
                            }
                        }
                        break;
                    case asState::AS_EMERGENCY: // when Emergency triggered abourt the mission
                        if(mission->m_missionState != MissionState::M_ABORTED){
                            res = mission -> abort();
                            mission -> remove_data_trigger();
                            mission -> stopMission();
                            mission -> switchAborted();
                            missionSelected = false;
                        }
                        break;
                    case asState::AS_FINISHED:
                        if(mission->m_missionState != MissionState::M_STOPPED){
                            mission -> remove_data_trigger();
                            mission -> stopMission();
                            mission -> switchStopped();
                            missionSelected = false;
                        }
                        break;
                    case asState::AS_OFF:
                        if(missionID != 0){
                            missionID = 0;
                            missionSelected = false;
                        }
                        break;
                    default: // do not care AS_OFF and AS_MANUAL and AS_FINISHED
                        res = true; // Do nothing
                }
            }
            return res;
        };
        // Finally, register the lambda as time-triggered function.
        od4.timeTrigger(frequency, missionStep); // Won't return until stopped.
    }
    return 0;
}