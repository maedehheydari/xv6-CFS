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

// Sorting fields
#define SORT_BY_PID 0
#define SORT_BY_VRUNTIME 1
#define SORT_BY_WEIGHT 2

// Helper Functions
int calc_len(int num);
void print_padded(const char *str, int width);
void print_number_padded(int num, int width, char *type);
void fetch_and_display(int sort_field);
void sort_procs(struct proc_info *infos, int count, int sort_field);

// Main visualizer function
int main(int argc, char *argv[]) {

    int sort_field = SORT_BY_PID;
    if (argc > 1) {
        if (strcmp(argv[1], "-v") == 0) {
            sort_field = SORT_BY_VRUNTIME;
        } else if (strcmp(argv[1], "-w") == 0) {
            sort_field = SORT_BY_WEIGHT;
        } else {
            sort_field = SORT_BY_PID;
        }
    }

    int pid = fork();

    if (pid == 0) {
        
        while (1) {
            fetch_and_display(sort_field);
            sleep(20);
        }
    } else {
        // Parent process: handle user input
        char input;
        while (1) {
            if (read(0, &input, 1) > 0) {
                if (input == 'q') {
                    kill(pid); // Kill the child process
                    wait(0);   // Wait for the child process to exit
                    printf("\nExiting visualizer...\n");
                    exit(0);
                }
            }
        }
    }

    return 0;
}

// Fetch process information and display it
void fetch_and_display(int sort_field) {
    struct proc_info infos[64]; // Buffer for proc_info structs
    int count = sysinfo(infos); // Fetch system-wide process info using sysinfo()

    if (count < 0) {
        printf("Failed to fetch process information.\n");
        return;
    }

    // Sort processes based on the selected field
    // sort_procs(infos, count, sort_field);

    // Clear the screen
    printf("\033[H\033[J"); // Clear the screen
    printf("=== Process Information ===\n\n");

    // Print table headers
    print_padded(sort_field == SORT_BY_PID ? "PID*" : "PID", 5);
    print_padded("Name", 16);
    print_padded(sort_field == SORT_BY_WEIGHT ? "Weight*" : "Weight", 8);
    print_padded("State", 10);
    print_padded(sort_field == SORT_BY_VRUNTIME ? "VRuntime*" : "VRuntime", 16);
    print_padded("Nice", 6);
    print_padded("Runtime", 10);
    printf("\n");
    for (int i = 0; i < 71; i++) printf("-"); // Horizontal separator
    printf("\n");

    // Print process information
    for (int i = 0; i < count; i++) {
        print_number_padded(infos[i].pid, 5, "%d");
        print_padded(infos[i].name, 16);
        print_number_padded(infos[i].weight, 8, "%d");
        print_padded(infos[i].state, 10);
        print_number_padded(infos[i].vruntime, 16, "%llu");
        print_number_padded(infos[i].nice, 6, "%d");
        print_number_padded(infos[i].runtime, 10, "%u");
        printf("\n");
    }
}

// Sort processes based on the selected field
void sort_procs(struct proc_info *infos, int count, int sort_field) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            int swap = 0;
            if (sort_field == SORT_BY_PID && infos[j].pid > infos[j + 1].pid) {
                swap = 1;
            } else if (sort_field == SORT_BY_VRUNTIME && infos[j].vruntime > infos[j + 1].vruntime) {
                swap = 1;
            } else if (sort_field == SORT_BY_WEIGHT && infos[j].weight > infos[j + 1].weight) {
                swap = 1;
            }

            if (swap) {
                struct proc_info temp = infos[j];
                infos[j] = infos[j + 1];
                infos[j + 1] = temp;
            }
        }
    }
}

// Calculate the length of a number
int calc_len(int num) {
    int len = 0;
    if (num < 0) {
        len++;
        num = -num;
    }
    do {
        len++;
        num /= 10;
    } while (num > 0);
    return len;
}

// Print a string padded to a fixed width
void print_padded(const char *str, int width) {
    printf("%s", str);
    for (int i = 0; i < width - strlen(str); i++) {
        printf(" ");
    }
}

// Print a number padded to a fixed width
void print_number_padded(int num, int width, char *type) {
    printf(type, num);
    int len = calc_len(num);
    for (int i = 0; i < width - len; i++) {
        printf(" ");
    }
}
