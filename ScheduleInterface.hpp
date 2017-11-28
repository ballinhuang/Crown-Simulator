#ifndef SCHEDULEINTERFACE_HPP
#define SCHEDULEINTERFACE_HPP

#include "Job.hpp"
#include <deque>
using namespace std;

class ScheduleInterface {
    public:
        virtual deque<Job> doSchedule(deque<Job>, deque<Job>, int, lli) = 0;
};

extern "C" {
    ScheduleInterface* getInstance();
}

#endif