#ifndef JOB_HPP
#define JOB_HPP

#define lli long long int

enum Type {
    wait, run, finish
};

class Job {
    public:
        lli getId();
        lli getSubmit();
        lli getRun();
        lli getWait();
        lli getEnd();
        int getNp();
        Type getType();

        friend class Simulator;

    private:
        Job(lli, lli, lli, lli, int, Type);

        lli id;
        lli submitTime;
        lli runTime;
        lli requestTime;
        lli waitTime;
        lli endTime;
        lli order;
        int np;
        Type type;
};

#endif