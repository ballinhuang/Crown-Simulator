#ifndef EASYBACKFILLING_HPP
#define EASYBACKFILLING_HPP

#include "ScheduleInterface.hpp"

class EASYBackfilling : public ScheduleInterface {
    public:
        EASYBackfilling();
        deque<Job> doSchedule(deque<Job>, deque<Job>, int, lli);

    private:
        static bool cmpJobEndtime(Job, Job);
};

#endif