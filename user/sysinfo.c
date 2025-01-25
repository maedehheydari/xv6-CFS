#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


// Define the structure for process info
struct proc_info {
    char name[16];    // Process name
    int pid;          // Process ID
    int weight;       // Process weight
    char state[16];   // State (e.g., RUNNING, SLEEPING)
    int nice;         // Niceness
    uint runtime;     // CPU time used
    uint64 vruntime;  // Virtual runtime
};


int
main(int argc, char **argv)
{
    struct proc_info information[64];
    int count = 0;

    if ((count=sysinfo(information)) < 0) {
        printf("sysinfo failed\n");
        exit(1);
    }
    
    printf("Total Running Processes: %d\n", count);
    printf("\nProcess Details:\n");
    printf("PID\tWeight\tVRT\tStatus\t\tName\n");
    printf("-----------------------------------------------------\n");
    
    for (int i = 0; i < count; i++) {
        struct proc_info *info = &information[i];
        printf("%d\t%d\t%lu\t%s\t%s\n", 
               info->pid, 
               info->weight, 
               info->vruntime, 
               info->state,
               info->name);
    }
    
    exit(0);
}
