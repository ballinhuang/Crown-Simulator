#include "Simulator.hpp"
#include <cstring>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <dlfcn.h>
#include <algorithm>
using namespace std;

Simulator::Simulator() {
    orderCounter = 0;
}

bool Simulator::finishQcmp(const Job a, const Job b) {
    return a.submitTime < b.submitTime;
}

void Simulator::storeOutcome() {
    stringstream ss;
    double sumWait = 0.0;

    sort(finishQ.begin(), finishQ.end(), finishQcmp);

    ss << setw(10) << "Job ID" << setw(15) << "Submit Time" << setw(15) << "Run Time" << setw(15) << "Wait Time" << setw(15) << "NP" << setw(15) << "Run Order" << endl;
    for(deque<Job>::iterator di = finishQ.begin(); di != finishQ.end(); di++) {
        ss << setw(10) << di->id << setw(15) << di->submitTime << setw(15) << di->runTime << setw(15) << di->waitTime << setw(15) << di->np << setw(15) << di->order << endl;
        sumWait += di->waitTime;
    }

    fstream output;

    fileName += ".result";
    output.open(fileName.c_str(), ios::out);
    if(!output.is_open()) {
        cout << "store result failed!" << endl;
        exit(1);
    }

    cout << "The sum of waiting time is " << fixed << setprecision(0) << sumWait << endl;
    output << "The sum of waiting time is " << fixed << setprecision(0) << sumWait << endl;
    cout << "The average waiting time is " << fixed << setprecision(2) << sumWait / finishQ.size() << endl;
    output << "The average waiting time is " << fixed << setprecision(2) << sumWait / finishQ.size() << endl;
    output << endl << ss.str();
}

void Simulator::executeJob(deque<Job> doJob) {
    for(deque<Job>::iterator di = doJob.begin(); di != doJob.end(); di++) {
        di->waitTime = timer - di->submitTime;
        di->order = ++orderCounter;
        allNp -= di->np;

        // insert to runQ
        di->type = run;
        runQ.push_back(*di);

        // insert to finishQ
        di->type = finish;
        finishQ.push_back(*di);

        // insert to eventQ
        bool flag = true;
        lli t = 0;
        for(deque<Job>::iterator dj = eventQ.begin(); dj != eventQ.end(); dj++) {
            if(dj->type == wait)
                t = dj->submitTime;
            else
                t = dj->getEnd();

            if(t >= di->getEnd()) {
                eventQ.insert(dj, *di);
                flag = false;
                break;
            }
        }
        if(flag)
            eventQ.push_back(*di);

        for(int i = 0; i < waitQ.size(); i++)
            if(di->id == waitQ[i].id) {
                waitQ.erase(waitQ.begin() + i);
                break;
            }
    }
}

void Simulator::removeRunJob(Job j) {
    for(deque<Job>::iterator di = runQ.begin(); di != runQ.end(); di++)
        if(di->id == j.id) {
            runQ.erase(di);
            break;
        }
}

void Simulator::loadScheduler(string mode) {
    ScheduleInterface* (*getInstance)();

    handle = dlopen(mode.c_str(), RTLD_LAZY);
    if(!handle) {
        cout << "handle error!" << endl;
        exit(1);
    }

    getInstance = (ScheduleInterface* (*)())dlsym(handle, "getInstance");
    if(dlerror() != NULL) {
        cout << "getInstance function loaded error!" << endl;
        exit(1);
    }
    scheduler = getInstance();
}

void Simulator::loadEvents() {
    fstream data;
    double temp = 0.0;
    lli counter = 0;
    
    data.open(fileName.c_str(), ios::in);
    if(!data.is_open()) {
        cout << "opening file failed!" << endl;
        exit(1);
    }

    while(data >> temp) {
        lli subT = 0, runT = 0;
        int np = 0;
        Type t;

        data >> subT;
        data >> temp;
        data >> runT;
        data >> np;

        for(int i = 0; i < 13; i++)
            data >> temp;

        Job j(++counter, subT, runT, np, wait);
        eventQ.push_back(j);
    }

    data.close();
}

void Simulator::fight(int argc, char **argv) {
    string scheduleMode = "./";

    for(int i = 1; i < argc; i++) {
        if(!strcmp(argv[i], "-f"))
            fileName = string(argv[++i]);
        else if(!strcmp(argv[i], "-np"))
            allNp = atoi(argv[++i]);
        else if(!strcmp(argv[i], "-m"))
            scheduleMode += string(argv[++i]);
    }

    // check number of all of np
    if(allNp < 1) {
        cout << "the resourse of np is wrong!" << endl;
        exit(1);
    }

    // load events in the log file
    loadEvents();
    cout << "eventQ's size is " << eventQ.size() << endl;
    
    // dynamic load the library
    loadScheduler(scheduleMode);

    string s = "";

    while(!eventQ.empty()) {
        Job j = eventQ.front();
        eventQ.pop_front();

        if(j.type == wait) {
            timer = j.submitTime;
            waitQ.push_back(j);
        }
        else {
            timer = j.getEnd();
            allNp += j.np;
            removeRunJob(j);
        }

        deque<Job> doJob = scheduler->doSchedule(waitQ, runQ, allNp, timer);
        executeJob(doJob);
    }

    storeOutcome();
    cout << "finishQ's size is " << finishQ.size() << endl;

    dlclose(handle);
}