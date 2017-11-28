#ifndef SJF_HPP
#define SJF_HPP

#include "ScheduleInterface.hpp"

class SJF : public ScheduleInterface {
    public:
        SJF();
        deque<Job> doSchedule(deque<Job>, deque<Job>, int, lli);

    private:
        static bool cmpJobRuntime(Job, Job);
};

#endif