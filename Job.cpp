#include "Job.hpp"

Job::Job(lli id, lli submit, lli run, lli req, int n, Type t) : id(id), submitTime(submit), runTime(run), requestTime(req), np(n), type(t) {
}

lli Job::getId() {
    return this->id;
}

lli Job::getSubmit() {
    return this->submitTime;
}
lli Job::getRun() {
    return this->requestTime;
}

lli Job::getWait() {
    return this->waitTime;
}

lli Job::getEnd() {
    if(this->type == wait)
        return -1;

    return this->submitTime + this->waitTime + this->requestTime;
}

int Job::getNp() {
    return this->np;
}

Type Job::getType() {
    return this->type;
}