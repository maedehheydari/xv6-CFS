#include "param.h"

struct proc_info {
    char name[16];
    int pid;
    int weight;
    uint64 vruntime;
};

struct sysinfo_data {
    uint32 running_processes;
    struct proc_info processes[NPROC];
};
