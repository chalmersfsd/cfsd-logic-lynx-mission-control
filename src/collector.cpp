/**
* Copyright (C) 2017 Chalmers Revere
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
* USA.
*/

#include "collector.hpp"

bool compareCone(const Cone& c1, const Cone& c2) {
  return (c1.m_x < c2.m_x);
}

/*-------------------------------------------------------*/
Collector::Collector(bool VERBOSE) : 
  m_currentConeFrame{}
, uncompleteFrameMutex{}
, uncompleteFrame{}
, currentUncompleteFrameId{}
, completeFrameMutex{}
, completeFrame{}
, currentCompleteFrameId{}
, frameStart{}
, frameEnd{}
, m_verbose{VERBOSE} 
{

}

void Collector::GetCompleteFrameCFSD19() {
    // Copy cones to m_currentConeFrame for processing
    std::lock_guard<std::mutex> lock(completeFrameMutex);
    if (!completeFrame.size())
        return;
        
    for (auto elem : completeFrame)
        m_currentConeFrame.push(elem.second);

    if (m_verbose) {
        uint64_t frameDuration = cluon::time::toMicroseconds(frameEnd) - 
                                 cluon::time::toMicroseconds(frameStart);
        std::cout << "Using frame to find path, frame duration=" << frameDuration
                  << "ms, #frames=" << completeFrame.size() <<  std::endl;
    }
}

void Collector::getObjectFrameStart(cluon::data::Envelope envelope) {
    opendlv::logic::perception::ObjectFrameStart msg =
        cluon::extractMessage<opendlv::logic::perception::ObjectFrameStart>(std::move(envelope));

    uint32_t objectFrameId = msg.objectFrameId();
    {
        std::lock_guard<std::mutex> lock(uncompleteFrameMutex);
        uncompleteFrame = std::map<uint32_t, Cone>();
        currentUncompleteFrameId = objectFrameId;
        frameStart = envelope.sent();
    }

    if (m_verbose) {
        std::cout << "Got frame START with id=" << objectFrameId << std::endl;
    }
}

void Collector::getObjectFrameEnd(cluon::data::Envelope envelope) {
    opendlv::logic::perception::ObjectFrameEnd msg =
        cluon::extractMessage<opendlv::logic::perception::ObjectFrameEnd>(std::move(envelope));

    uint32_t objectFrameId = msg.objectFrameId();
    {
        std::lock_guard<std::mutex> lock1(completeFrameMutex);
        std::lock_guard<std::mutex> lock2(uncompleteFrameMutex);
        completeFrame = uncompleteFrame;

        currentCompleteFrameId = currentUncompleteFrameId;
        frameEnd = envelope.sent();
    }

    if (m_verbose) {
        std::cout << "Got frame END with id=" << objectFrameId << std::endl;
    }
}

void Collector::getObjectType(cluon::data::Envelope envelope) {
    opendlv::logic::perception::ObjectType msg =
        cluon::extractMessage<opendlv::logic::perception::ObjectType>(std::move(envelope));

    uint32_t objectId = msg.objectId();
    {
        std::lock_guard<std::mutex> lock(uncompleteFrameMutex);
        Cone newCone(objectId, 0, 0, 0);
        uncompleteFrame[objectId] = newCone;
        std::cout << "Got NEW OBJECT with id=" << objectId << std::endl;

        if (uncompleteFrame.count(objectId)) {
            uncompleteFrame[objectId].m_color = msg.type();
        }
    }
    if (m_verbose) {
        std::cout << "Got OBJECT TYPE for object with id=" << objectId
                  << " and type=" << msg.type() << std::endl;
    }
}

void Collector::getObjectPosition(cluon::data::Envelope envelope) {
    opendlv::logic::perception::ObjectPosition msg =
        cluon::extractMessage<opendlv::logic::perception::ObjectPosition>(std::move(envelope));

    uint32_t objectId = msg.objectId();
    {
        std::lock_guard<std::mutex> lock(uncompleteFrameMutex);
        if (uncompleteFrame.count(objectId)) {
            // remap the coordiate so that the coordinate is as follow in path planner
            // from perception:
            // x: positive upward vertically
            // y: positive to left horizontally
            uncompleteFrame[objectId].m_x = msg.x();
            uncompleteFrame[objectId].m_y = msg.y();
            
            // In path planner
            // x: postive to right horizontally
            // y: positive upward vertically
            // uncompleteFrame[objectId].m_x = -msg.y();
            // uncompleteFrame[objectId].m_y = msg.x();
        }
    }
    if (m_verbose) {
        std::cout << "Got OBJECT POSITION for object with id=" << objectId
                  << " and x=" << msg.x() << " y=" << msg.y() << std::endl;
    }
}

void Collector::getEquilibrioception(cluon::data::Envelope envelope) {
    opendlv::logic::sensation::Equilibrioception msg =
        cluon::extractMessage<opendlv::logic::sensation::Equilibrioception>(std::move(envelope));

    float vx = msg.vx();
    float yawRate = msg.yawRate();
    if (m_verbose) {
        std::cout << "Got EQUILIBRIOCEPTION vx=" << vx << " and yawRate="
                  << yawRate << std::endl;
    }
}

int Collector::ProcessFrameCFSD19() {
    if (!m_currentConeFrame.size()) {
        std::cout << "Current frame has no cones!" << std::endl;
        return 0;
    }

    if (m_verbose)
        std::cout << "Current frame has: " << m_currentConeFrame.size() << " cones\n";
    
    // Copy cones of different colors to their own containers for processing
    // std::vector<Cone> tempYellowCones;
    // std::vector<Cone> tempBlueCones;
    // std::vector<Cone> tempOrangeCones;     
    
    int numBlueCones = 0;
    int numYellowCones = 0;
    int numOrangeCones = 0;
    while(m_currentConeFrame.size() >0) {
        Cone cone = m_currentConeFrame.front();
        switch (m_currentConeFrame.front().m_color) {
            case 0: // yellow
                numYellowCones++;
                // tempYellowCones.push_back(cone);
                break;
            case 1: // blue
                numBlueCones++;
                // tempBlueCones.push_back(cone);
                break;
            case 2: // orange
                numOrangeCones++;
                // tempOrangeCones.push_back(cone);
                break;
        }
        // Done copying, delete pointers to free memory
        m_currentConeFrame.pop();
    }
    if (m_verbose) {
        std::cout << "number of cones:" 
                  << "\n    yellow " << numYellowCones
                  << "\n      blue " << numBlueCones
                  << "\n    orange " << numOrangeCones
                  << std::endl;
    }

    return numOrangeCones;
}