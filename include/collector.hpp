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

#ifndef COLLECTOR_HPP
#define COLLECTOR_HPP

#include <cmath>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <mutex>
#include <vector>
#include <queue>
#include <iterator>

#include "cluon-complete.hpp"
#include "opendlv-standard-message-set.hpp"

struct Cone{
    Cone():
        m_objectId(0), m_x(0.0), m_y(0.0), m_color(0) {}
    
    Cone(uint32_t objectId, double x, double y,uint32_t color): 
        m_objectId(objectId), m_x(x), m_y(y), m_color(color) {}
    
    ~Cone() = default;
    
    uint32_t m_objectId;
    double m_x;
    double m_y;
    uint32_t m_color;
};

bool compareCone(const Cone& c1, const Cone& c2);

class Collector{
  public:
    Collector(bool);
    ~Collector() = default;

    // CFSD19 modification
    void CollectConesCFSD19();
    void GetCompleteFrameCFSD19();
    void ProcessFrameCFSD19(bool goRight);
    
  public:
    std::queue<Cone> m_currentConeFrame;
    
    // Variables for creating cone frames
    std::mutex uncompleteFrameMutex;
    std::map<uint32_t, Cone> uncompleteFrame;
    uint32_t currentUncompleteFrameId;

    std::mutex completeFrameMutex;
    std::map<uint32_t, Cone> completeFrame;
    uint32_t currentCompleteFrameId;
    
    cluon::data::TimeStamp frameStart;
    cluon::data::TimeStamp frameEnd;
    bool m_verbose;

    // Function for creating cone frames
    void getObjectFrameStart(cluon::data::Envelope envelope);
    void getObjectFrameEnd(cluon::data::Envelope envelope);
    void getObjectType(cluon::data::Envelope envelope);
    void getObjectPosition(cluon::data::Envelope envelope);
    void getEquilibrioception(cluon::data::Envelope envelope);    
};

#endif
