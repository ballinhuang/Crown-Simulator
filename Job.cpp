#include "Job.hpp"

Job::Job(lli id, lli submit, lli run, int n, Type t) : id(id), submitTime(submit), runTime(run), np(n), type(t) {
}

void Job::calculateEnd() {
    endTime = submitTime + runTime + waitTime;
}