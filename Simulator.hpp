#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include "Job.hpp"
#include "ScheduleInterface.hpp"
#include <deque>
#include <string>
using namespace std;

class Simulator {
    public:
        Simulator();
        void fight(int, char **);

    private:
        static bool finishQcmp(const Job, const Job);
        void storeOutcome();
        void executeJob(deque<Job>);
        void removeRunJob(Job);
        void loadScheduler(string);
        void loadEvents();

        ScheduleInterface *scheduler;
        string fileName;
        int allNp;
        lli timer;
        lli orderCounter;
        deque<Job> eventQ;
        deque<Job> waitQ;
        deque<Job> runQ;
        deque<Job> finishQ;
        void *handle;
};



#endif