#include "Job.hpp"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <fstream>
#include <sstream>
#include <dlfcn.h>
#include <cstdlib>
#include <deque>
#include <algorithm>
using namespace std;

void storeOutcome(deque<Job> finishQ, string fileName) {
    stringstream ss;
    double sumWait = 0.0;

    ss << setw(10) << "Job ID" << setw(15) << "Submit Time" << setw(15) << "Run Time" << setw(15) << "Wait Time" << setw(15) << "NP" << endl;
    for(deque<Job>::iterator di = finishQ.begin(); di != finishQ.end(); di++) {
        ss << setw(10) << di->id << setw(15) << di->submitTime << setw(15) << di->runTime << setw(15) << di->waitTime << setw(15) << di->np << endl;
        sumWait += di->waitTime;
    }

    fstream output;

    fileName += ".outcome";
    output.open(fileName.c_str(), ios::out);
    if(!output.is_open()) {
        cout << "store outcome failed!" << endl;
        exit(1);
    }

    cout << "The sum of waiting time is " << fixed << sumWait << endl;
    cout << "The average waiting time is " << fixed << sumWait / finishQ.size() << endl;
    output << endl;
    output << ss.str();
}

bool jobCmp(Job a, Job b) {
    lli ta = 0, tb = 0;

    if(a.type == wait)
        ta = a.submitTime;
    else
        ta = a.submitTime + a.waitTime + a.runTime;

    if(b.type == wait)
        tb = b.submitTime;
    else
        tb = b.submitTime + b.waitTime + b.runTime;

    return ta < tb;
}

bool subtractNP(deque<Job> doJob, int &NP, deque<Job> &eventQ, deque<Job> &finishQ, lli currentTime) {
    bool isEventUpdate = false;

    while(!doJob.empty()) {
        Job j = doJob.front();
        doJob.pop_front();

        NP -= j.np;
        j.type = finish;
        j.waitTime = currentTime - j.submitTime;
        j.calculateEnd();
        eventQ.push_front(j);
        finishQ.push_back(j);
        isEventUpdate = true;
    }

    return isEventUpdate;
}

void removeWaitQ(deque<Job> doJob, deque<Job> &wait) {
    for(deque<Job>::iterator di = doJob.begin(); di != doJob.end(); di++)
        for(int i = 0; i < wait.size(); i++)
            if(di->id == wait[i].id) {
                wait.erase(wait.begin() + i);
                break;
            }
}

int main(int argc, char **argv) {
    string fileName = "", scheduleMode = "";
    fstream data;
    int currentNp = 0;

    for(int i = 1; i < argc; i++) {
        if(!strcmp(argv[i], "-f"))
            fileName = string(argv[++i]);
        else if(!strcmp(argv[i], "-np"))
            currentNp = atoi(argv[++i]);
        else if(!strcmp(argv[i], "-m"))
            scheduleMode = string(argv[++i]);
    }

    data.open(fileName.c_str(), ios::in);
    if(!data.is_open()) {
        cout << "opening file failed!" << endl;
        exit(1);
    }
    if(currentNp < 1) {
        cout << "the resourse of np is wrong!" << endl;
        exit(1);
    }

    void *handle;
    deque<Job> (*doSchedule)(deque<Job>, int);

    // dynamic load the library
    handle = dlopen(scheduleMode.c_str(), RTLD_LAZY);
    if(!handle) {
        cout << "handle error!" << endl;
        exit(1);
    }

    doSchedule = (deque<Job>(*)(deque<Job>, int))dlsym(handle, "doSchedule");
    if(dlerror() != NULL) {
        cout << "doSchedule function loaded error!" << endl;
        exit(1);
    }

    deque<Job> eventQ, waitQ, finishQ;
    double temp = 0.0;
    lli counter = 0, currentTime = 0;
    string s = "";

    while(data >> temp) {
        lli sub = 0, run = 0;
        int np = 0;
        Type t;

        data >> sub;
        data >> temp;
        data >> run;
        data >> np;

        for(int i = 0; i < 13; i++)
            data >> temp;

        Job j(++counter, sub, run, np, wait);
        eventQ.push_back(j);
    }
    cout << "eventQ's size is " << eventQ.size() << endl;

    while(!eventQ.empty()) {
        Job j = eventQ.front();
        eventQ.pop_front();

        if(j.type == wait) {
            currentTime = j.submitTime;
            waitQ.push_back(j);
        }
        else {
            currentTime = j.endTime;
            currentNp += j.np;
        }

        deque<Job> doJob = doSchedule(waitQ, currentNp);
        removeWaitQ(doJob, waitQ);
        if(subtractNP(doJob, currentNp, eventQ, finishQ, currentTime))
            sort(waitQ.begin(), waitQ.end(), jobCmp);
    }

    storeOutcome(finishQ, fileName);

    cout << "finishQ's size is " << finishQ.size() << endl;

    return 0;
}