#include "EASYBackfilling.hpp"
#include <algorithm>
#include <iostream>
using namespace std;

ScheduleInterface* getInstance() {
    return new EASYBackfilling;
}

EASYBackfilling::EASYBackfilling() {}

deque<Job> EASYBackfilling::doSchedule(deque<Job> wait, deque<Job> run, int currentNp, lli currentTime) {
    deque<Job> result;
    Job j = wait.front();

    while(!wait.empty()) {
        j = wait.front();

        if(j.getNp() <= currentNp) {
            result.push_back(j);
            run.push_back(j);
            wait.pop_front();
            currentNp -= j.getNp();
        }
        else
            break;
    }

    if(wait.empty())
        return result;

    // do easy backfilling
    int temp = currentNp, minNp = 0;
    lli shadowTime = 0;

    sort(run.begin(), run.end(), cmpJobEndtime);
    for(deque<Job>::iterator di = run.begin(); di != run.end(); di++) {
        temp += di->getNp();
        if(temp >= j.getNp()) {
            shadowTime = di->getEnd();
            temp -= j.getNp();
            if(temp < currentNp)
                minNp = temp;
            else
                minNp = currentNp;

            break;
        }
    }

    wait.pop_front();
    for(deque<Job>::iterator di = wait.begin(); di != wait.end() && currentNp > 0; di++) {
        if((di->getNp() <= minNp) || (di->getNp() <= currentNp && currentTime + di->getRun() <= shadowTime)) {
            result.push_back(*di);
            currentNp -= di->getNp();
            if(currentNp < minNp)
                minNp = currentNp;
        }
    }

    return result;
}

bool EASYBackfilling::cmpJobEndtime(Job a, Job b) {
    return a.getEnd() < b.getEnd();
}