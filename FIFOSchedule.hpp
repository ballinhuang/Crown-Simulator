#ifndef FIFOSCHEDULE_HPP
#define FIFOSCHEDULE_HPP

#include "Job.hpp"
#include <deque>
using namespace std;

extern "C" deque<Job> doSchedule(deque<Job>, int);

#endif