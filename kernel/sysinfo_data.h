#include "param.h"

struct proc_info {
    char name[16];
    int pid;
    int weight;
    char state[16];   // State (e.g., RUNNING, SLEEPING)
    int nice;         // Niceness
    uint runtime;     // CPU time used
    uint64 vruntime;
};
