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
    webRequest = false;
}

bool Simulator::finishQcmp(const Job a, const Job b) {
    return a.id < b.id;
}

void Simulator::storeOutcome() {
    double sumWait = 0.0, sumTurnaround = 0.0;

    sort(finishQ.begin(), finishQ.end(), finishQcmp);

    for(deque<Job>::iterator di = finishQ.begin(); di != finishQ.end(); di++) {
        sumWait += di->waitTime;
        sumTurnaround += di->waitTime + di->runTime;
    }

    if(webRequest) {
        cout << fixed << setprecision(2) << sumWait / finishQ.size() << endl;
        cout << fixed << setprecision(2) << sumTurnaround / finishQ.size() << endl;
        for(deque<Job>::iterator di = finishQ.begin(); di != finishQ.end(); di++)
            cout << " " << di->id << " " << di->submitTime << " " << (di->runTime > di->requestTime ? di->requestTime : di->runTime) << " " << di->waitTime << " " << di->np << " " << di->order << endl;

        return;
    }

    fstream output;

    output.open((fileName + ".result").c_str(), ios::out);
    if(!output.is_open()) {
        cout << "store result failed!" << endl;
        exit(1);
    }

    output << "The average waiting time is " << fixed << setprecision(2) << sumWait / finishQ.size() << endl;
    output << "The average turnaround time is " << fixed << setprecision(2) << sumTurnaround / finishQ.size() << endl;

    output << setw(10) << "Job ID" << setw(15) << "Submit Time" << setw(15) << "Run Time" << setw(15) << "Wait Time" << setw(15) << "NP" << setw(15) << "Run Order" << endl;
    for(deque<Job>::iterator di = finishQ.begin(); di != finishQ.end(); di++)
        output << setw(10) << di->id << setw(15) << di->submitTime << setw(15) << (di->runTime > di->requestTime ? di->requestTime : di->runTime) << setw(15) << di->waitTime << setw(15) << di->np << setw(15) << di->order << endl;
}

void Simulator::executeJob(deque<Job> doJob) {
    for(deque<Job>::iterator di = doJob.begin(); di != doJob.end(); di++) {
        di->waitTime = timer - di->submitTime;
        di->endTime = timer + di->runTime;
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
                t = dj->endTime;

            if(t >= di->endTime) {
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
    
    data.open(fileName.c_str(), ios::in);
    if(!data.is_open()) {
        cout << "opening file failed!" << endl;
        exit(1);
    }


    double temp = 0.0;
    lli id = 0, subT = 0, runT = 0, reqT = 0;
    int np = 0, status = 0;

    while(data >> id) {
        data >> subT;
        data >> temp;
        data >> runT;
        data >> np;
        for(int i = 6; i <= 8; i++)
            data >> temp;
        data >> reqT;
        data >> temp;
        data >> status;
        for(int i = 12; i <= 18; i++)
            data >> temp;

        if(status != 1)
            continue;

        if(reqT == -1)
            reqT = runT;
        
        Job j(id, subT, runT, reqT, np, wait);
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
        else if(!strcmp(argv[i], "-web"))
            webRequest = true;
    }

    // check number of all of np
    if(allNp < 1) {
        cout << "the resourse of np is wrong!" << endl;
        exit(1);
    }

    // load events in the log file
    loadEvents();
    
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
            timer = j.endTime;
            allNp += j.np;
            removeRunJob(j);
        }

        deque<Job> doJob = scheduler->doSchedule(waitQ, runQ, allNp, timer);
        executeJob(doJob);
    }

    storeOutcome();

    dlclose(handle);
}