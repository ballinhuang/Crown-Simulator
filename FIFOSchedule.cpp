#include "FIFOSchedule.hpp"

deque<Job> doSchedule(deque<Job> wait, int currentNp) {
    deque<Job> result;

    while(!wait.empty()) {
        Job j = wait.front();
        wait.pop_front();

        if(j.np <= currentNp) {
            result.push_back(j);
            currentNp -= j.np;
        }
        else
            break;
    }

    return result;
}