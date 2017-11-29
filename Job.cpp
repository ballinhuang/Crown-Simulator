#include "Job.hpp"

Job::Job(lli id, lli submit, lli run, int n, Type t) : id(id), submitTime(submit), runTime(run), np(n), type(t) {
}

lli Job::getId() {
    return this->id;
}

lli Job::getSubmit() {
    return this->submitTime;
}
lli Job::getRun() {
    return this->runTime;
}

lli Job::getWait() {
    return this->waitTime;
}

lli Job::getEnd() {
    if(this->type == wait)
        return -1;

    return this->submitTime + this->waitTime + this->runTime;
}

int Job::getNp() {
    return this->np;
}

Type Job::getType() {
    return this->type;
}