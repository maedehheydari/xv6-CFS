#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <cpu_id>\n", argv[0]);
        exit(1);
    }
    int cpu_id = atoi(argv[1]);
    return cpupin(cpu_id);
}
