#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/sysinfo_data.h"

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
    printf("PID\tWeight\tVRT\tLT\tART\tName\n");
    printf("---------------------------------------------------\n");
    
    for (int i = 0; i < count; i++) {
        struct proc_info *info = &information[i];
            printf("%d\t%d\t%lu\t%lu\t%lu\t%s\n", 
               info->pid, 
               info->weight, 
               info->vruntime, 
               info->lifetime,
               info->runtime,
               info->name);
    }
  
    exit(0);
}
