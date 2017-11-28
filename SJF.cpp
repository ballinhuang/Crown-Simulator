#include "SJF.hpp"
#include <algorithm>

ScheduleInterface* getInstance() {
    return new SJF;
}

SJF::SJF() {}

deque<Job> SJF::doSchedule(deque<Job> wait, deque<Job> run, int currentNp, lli currentTime) {
    deque<Job> result;

    sort(wait.begin(), wait.end(), cmpJobRuntime);

    for(deque<Job>::iterator di = wait.begin(); di != wait.end(); di++) {
        if(di->getNp() <= currentNp) {
            result.push_back(*di);
            currentNp -= di->getNp();
        }
        else
            break;
    }

    return result;
}

bool SJF::cmpJobRuntime(Job a, Job b) {
    return a.getRun() < b.getRun();
}