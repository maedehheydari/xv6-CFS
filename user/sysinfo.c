#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/sysinfo_data.h"

int
main(int argc, char **argv)
{
    struct sysinfo_data information;
    if (sysinfo(&information) < 0) {
        printf("sysinfo failed\n");
        exit(1);
    }
    
    printf("Total Running Processes: %d\n", information.running_processes);
    printf("\nProcess Details:\n");
    printf("PID\tWeight\tVruntime\tName\n");
    printf("----------------------------------------\n");
    
    for (int i = 0; i < information.running_processes; i++) {
        struct proc_info *info = &information.processes[i];
        printf("%d\t%d\t%lu\t%s\n", 
               info->pid, 
               info->weight, 
               info->vruntime, 
               info->name);
    }
    
    exit(0);
}
