#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

struct proc_info {
    char name[16];
    int pid;
    int weight;
    char state[16];   // State (e.g., RUNNING, SLEEPING)
    int nice;         // Niceness
    uint runtime;     // CPU time used
    uint64 vruntime;
    uint64 lifetime;  // Lifetime of the process
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
    printf("PID\tWeight\tVruntime\tName\tLifetime\n");
    printf("----------------------------------------\n");
    
    for (int i = 0; i < count; i++) {
        struct proc_info *info = &information[i];
            printf("%d\t%d\t%lu\t%s\t%lu\n", 
               info->pid, 
               info->weight, 
               info->vruntime, 
               info->name,
               info->lifetime);
    }
  
    exit(0);
}
