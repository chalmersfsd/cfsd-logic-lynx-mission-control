#ifndef SKIDPAD_H
#define SKIDPAD_H


#include "Mission-control.hpp"
#include "cluon-complete.hpp"
#include "opendlv-standard-message-set.hpp"
#include "cfsd-extended-message-set.hpp"
/********
 * 
 */
class Skidpad: public MissionControl
{
private:
    /* data */
    
public:
    Skidpad(cluon::OD4Session&);
    ~Skidpad();
    bool create_data_trigger(); 
    bool remove_data_trigger();
    bool wait();
    bool init();
    bool step();
    bool abort();
};

#endif // SKIDPAD_H