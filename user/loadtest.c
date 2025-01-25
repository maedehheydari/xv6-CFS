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
cpu_intensive_test(int workload) {
    int i;   

    for(i = 0; i < workload; i++){
        int pid = fork();
        if(pid < 0){
            printf("fork failed\n");
            return -1;
        } else if(pid == 0){
            char *args[] = {"cpuheavy", 0};
            exec("cpuheavy", args);
            printf("exec cpuheavy failed\n");
            return -1;
        }
    }

    for(i = 0; i < workload; i++){
        wait(0);
    }

    printf("\n");
    printf("All cpuheavy processes finished.\n");
    return 0;
}

int
io_intensive_test(int workload) {
    int i;   

    for(i = 0; i < workload; i++){
        int pid = fork();
        if(pid < 0){
            printf("fork failed\n");
            return -1;
        } else if(pid == 0){
            char *args[] = {"ioheavy", 0};
            exec("ioheavy", args);
            printf("exec ioheavy failed\n");
            return -1;
        }
    }

    for(i = 0; i < workload; i++){
        wait(0);
    }

    printf("\n");
    printf("All ioheavy processes finished.\n");
    return 0;
}

int
mixed_workload_test(int workload) {
    int i, n;
    
    if (workload < 30) {
        n = 2*workload;
    } else {
        n = workload;
    }

    for(i = 0; i < n; i++){
        int pid = fork();
        if(pid < 0){
            printf("fork failed\n");
            return -1;
        } else if(pid == 0){
            if(i % 2 == 0){
                char *args[] = {"cpuheavy", 0};
                exec("cpuheavy", args);
                printf("exec cpuheavy failed\n");
                return -1;
            } else {
                char *args[] = {"ioheavy", 0};
                exec("ioheavy", args);
                printf("exec ioheavy failed\n");
                return -1;
            }
        }
    }

    for(i = 0; i < n; i++){
        wait(0);
    }
    printf("\n");
    printf("All mixed workload processes finished.\n");
    return 0;
}

int
run_tests(char *test_type, int workload){
    if(strcmp(test_type, "cpu") == 0){
        return cpu_intensive_test(workload);
    } else if(strcmp(test_type, "io") == 0){
        return io_intensive_test(workload);
    } else if(strcmp(test_type, "mixed") == 0){
        return mixed_workload_test(workload);
    } else {
        printf("Invalid test type\n");
        return -1;
    }
}

int main(int argc, char *argv[]) {
    char *test_types[3] = {"io", "cpu", "mixed"};
    // int workloads[3] = {5, 10, 15};
    int workloads[3] = {1, 0, 0};
    // struct proc_info infos[100][64];
    // int counts[100];

    int logger_pid = fork();
    if (logger_pid < 0) {
        printf("fork failed\n");
        return -1;
    } else if (logger_pid == 0) {
        for (int i = 0; i < 100; i++) {
            int pid = fork();
            if(pid == 0){
                char *args[] = {"sysinfo", 0};
                exec("sysinfo", args);
                printf("exec logger failed\n");
                exit(1);

            } else if (pid < 0) {
                printf("fork failed\n");
                exit(1);

            } else if (pid > 0) {
                wait(0);
            }
            sleep(20);
        }
        exit(0);
    } else if (logger_pid > 0) {
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                printf("\n");
                printf("Running %s test with workload %d\n", test_types[i], workloads[j]);
                run_tests(test_types[i], workloads[j]);
            }
        }

        kill(logger_pid);
    }

    return 0;
}