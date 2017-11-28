#ifndef FIFOSCHEDULE_HPP
#define FIFOSCHEDULE_HPP

#include "ScheduleInterface.hpp"

class FIFO : public ScheduleInterface {
    public:
        FIFO();
        deque<Job> doSchedule(deque<Job>, deque<Job>, int, lli);
};

#endif