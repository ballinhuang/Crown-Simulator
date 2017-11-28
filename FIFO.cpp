#include "FIFO.hpp"

ScheduleInterface* getInstance() {
    return new FIFO;
}

FIFO::FIFO() {}

deque<Job> FIFO::doSchedule(deque<Job> wait, deque<Job> run, int currentNp, lli currentTime) {
    deque<Job> result;

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