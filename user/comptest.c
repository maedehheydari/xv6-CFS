#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/sysinfo_data.h"

#define max(a, b) ((a) > (b) ? (a) : (b))

struct proc_info comparison_infos[2][10][64]; 
int counts[2][10];

// Print a string padded to a fixed width
void print_padded(const char *str, int width) {
    printf("%s", str);
    for (int i = 0; i < width - strlen(str); i++) {
        printf(" ");
    }
}

int
mixed_workload_test(int workload) {
    int i;

    for(i = 0; i < workload; i++){
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

    for(i = 0; i < workload; i++){
        wait(0);
    }
    return 0;
}

void print_results() {
    printf("\nPrinting results...\n");
    struct proc_info *info;

    for (int i = 0; i < 10; i++) {
        printf("\n");
        printf("Total Running Processes: CFS=%d | RR=%d\n", counts[0][i], counts[1][i]);
        printf("PID\tWeight\tVRT\tLT\tART\t");
        print_padded("Name", 10);
        print_padded("Status", 10);
        printf("|\tPID\tWeight\tVRT\tLT\tART\t");
        print_padded("Name", 10);
        print_padded("Status", 10);
        printf("\n");

        printf("--------------------------------------------------------------------------------------------------------------------------\n");
        
        for (int j = 0; j < max(counts[0][i], counts[1][j]); j++) {
            if (j >= counts[0][i]) {
                printf("   \t     \t   \t  \t   \t");
                print_padded(" ", 10);
                print_padded(" ", 10);

            } else {
                info = &comparison_infos[0][i][j];
                printf("%d\t%d\t%lu\t%lu\t%lu\t", 
                    info->pid, 
                    info->weight, 
                    info->vruntime, 
                    info->lifetime,
                    info->runtime);

                print_padded(info->name, 10);
                print_padded(info->state, 10);
            }

            printf("|\t");

            if (j >= counts[1][i]){
                printf("   \t     \t   \t  \t   \t");
                print_padded(" ", 10);
                print_padded(" ", 10);
            } else {
                info = &comparison_infos[1][i][j];
                printf("%d\t%d\t%lu\t%lu\t%lu\t", 
                    info->pid, 
                    info->weight, 
                    info->vruntime, 
                    info->lifetime,
                    info->runtime);
                print_padded(info->name, 10);
                print_padded(info->state, 10);
            }
            
            printf("\n");
        }

    }
    
}

int main(int argc, char *argv[]) {

    int logger_pid = fork();
    if (logger_pid < 0) {
        printf("fork failed\n");
        return -1;
    } else if (logger_pid == 0) {
        for (int i = 0; i < 20; i++) {
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
            sleep(10);
        }
        exit(0);
    } else if (logger_pid > 0) {
        int runner_pid = fork();
        
        if (runner_pid < 0) {
            printf("fork failed\n");
            return -1;
        } else if (runner_pid == 0) {
            mixed_workload_test(20);
            exit(0);
        } else if (runner_pid > 0) {
            for (int i = 0; i < 10; i++) {
                counts[0][i] = sysinfo(comparison_infos[0][i]);
                sleep(10);
            }
            for (int i = 0; i < 10; i++) {
                counts[1][i] = sysinfo(comparison_infos[1][i]);
                sleep(10);
            }
            wait(0);
            wait(0);
            print_results();
        }
    }

    return 0;
}