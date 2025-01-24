#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/sysinfo_data.h"

int
main(int argc, char **argv)
{
    struct sysinfo_data information;
    sysinfo(&information);
    printf("Running Processes: %d\n", information.running_processes);
    exit(0);
}
