#include "param.h"

struct proc_info {
    char name[16];
    int pid;
    int weight;
    char state[8];
    uint64 runtime;
    uint64 vruntime;
    uint64 lifetime;
};
