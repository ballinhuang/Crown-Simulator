#include "Job.hpp"

Job::Job(lli id, lli submit, lli run, int n, Type t) : id(id), submitTime(submit), runTime(run), np(n), type(t) {
}

lli Job::getId() {
    return id;
}

lli Job::getSubmit() {
    return submitTime;
}
lli Job::getRun() {
    return runTime;
}

lli Job::getWait() {
    return waitTime;
}

lli Job::getEnd() {
    return submitTime + waitTime + runTime;
}

int Job::getNp() {
    return np;
}

Type Job::getType() {
    return type;
}