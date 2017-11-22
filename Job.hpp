#ifndef JOB_HPP
#define JOB_HPP

#define lli long long int

enum Type {
    wait, finish
};

class Job {
    public:
        Job(lli, lli, lli, int, Type);
        void calculateEnd();

        lli id;
        lli submitTime;
        lli runTime;
        lli waitTime;
        lli endTime;
        int np;
        Type type;
};

#endif