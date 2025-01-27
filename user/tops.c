#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/sysinfo_data.h"

// Sorting fields
#define SORT_BY_PID 0
#define SORT_BY_VRUNTIME 1
#define SORT_BY_WEIGHT 2
#define SORT_BY_ART 3
#define SORT_BY_LT 4

#define SORT_ASC 0
#define SORT_DESC 1

// Helper Functions
void print_padded(const char *str, int width);
void fetch_and_display(int sort_field, int sort_order);
void sort_procs(struct proc_info *infos, int count, int sort_field, int sort_order);

// Main visualizer function
int main(int argc, char *argv[]) {

    int sort_field = SORT_BY_PID;
    int sort_order = SORT_ASC;
    
    char input[2];
    while (1) {
        fetch_and_display(sort_field, sort_order);
        sleep(10);

        if (async_read(input, 1) > 0) {
            if (input[0] == 'q') {
                printf("\nExiting visualizer...\n");
                exit(0);
            } else if (input[0] == 'v') {
                sort_field = SORT_BY_VRUNTIME;
            } else if (input[0] == 'w') {
                sort_field = SORT_BY_WEIGHT;
            } else if (input[0] == 'p') {
                sort_field = SORT_BY_PID;
            } else if (input[0] == 'a') {
                sort_field = SORT_BY_ART;
            } else if (input[0] == 'l') {
                sort_field = SORT_BY_LT;
            } else if (input[0] == '+') {
                sort_order = SORT_ASC;
            } else if (input[0] == '-') {
                sort_order = SORT_DESC;
            }
        }
    }
    return 0;
}

// Fetch process information and display it
void fetch_and_display(int sort_field, int sort_order) {
    struct proc_info infos[64];
    int count = sysinfo(infos);
    struct proc_info *info;

    if (count < 0) {
        printf("Failed to fetch process information.\n");
        return;
    }

    // Sort processes based on the selected field
    sort_procs(infos, count, sort_field, sort_order);

    // Clear the screen
    printf("\033[H\033[J"); // Clear the screen
    printf("=== Process Information ===\n\n");

    // Print table headers
    print_padded(sort_field == SORT_BY_PID ? sort_order == SORT_ASC ? "PID+" : "PID-" : "PID", 8);
    print_padded(sort_field == SORT_BY_WEIGHT ? sort_order == SORT_ASC ? "Weight+" : "Weight-" : "Weight", 8);
    print_padded(sort_field == SORT_BY_VRUNTIME ? sort_order == SORT_ASC ? "VRT+" : "VRT-" : "VRT", 8);
    print_padded(sort_field == SORT_BY_LT ? sort_order == SORT_ASC ? "LT+" : "LT-" : "LT", 8);
    print_padded(sort_field == SORT_BY_ART ? sort_order == SORT_ASC ? "ART+" : "ART-" : "ART", 8);
    print_padded("Name", 10);
    print_padded("State", 10);
    printf("\n");

    for (int i = 0; i < 70; i++) printf("-");
    printf("\n");
    
    for (int i = 0; i < count; i++) {
        info = &infos[i];
        printf("%d\t%d\t%lu\t%lu\t%lu\t", 
            info->pid, 
            info->weight, 
            info->vruntime, 
            info->lifetime,
            info->runtime);

        print_padded(info->name, 10);
        print_padded(info->state, 10);
        printf("\n");
    }
}

// Sort processes based on the selected field
void sort_procs(struct proc_info *infos, int count, int sort_field, int sort_order) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            int swap = 0;
            if (sort_order==SORT_ASC) {
                if (sort_field == SORT_BY_PID && infos[j].pid > infos[j + 1].pid) {
                    swap = 1;
                } else if (sort_field == SORT_BY_VRUNTIME && infos[j].vruntime > infos[j + 1].vruntime) {
                    swap = 1;
                } else if (sort_field == SORT_BY_WEIGHT && infos[j].weight > infos[j + 1].weight) {
                    swap = 1;
                } else if (sort_field == SORT_BY_ART && infos[j].runtime > infos[j + 1].runtime) {
                    swap = 1;
                } else if (sort_field == SORT_BY_LT && infos[j].lifetime > infos[j + 1].lifetime) {
                    swap = 1;
                }
            } else if (sort_order==SORT_DESC) {
                if (sort_field == SORT_BY_PID && infos[j].pid < infos[j + 1].pid) {
                    swap = 1;
                } else if (sort_field == SORT_BY_VRUNTIME && infos[j].vruntime < infos[j + 1].vruntime) {
                    swap = 1;
                } else if (sort_field == SORT_BY_WEIGHT && infos[j].weight < infos[j + 1].weight) {
                    swap = 1;
                } else if (sort_field == SORT_BY_ART && infos[j].runtime < infos[j + 1].runtime) {
                    swap = 1;
                } else if (sort_field == SORT_BY_LT && infos[j].lifetime < infos[j + 1].lifetime) {
                    swap = 1;
                }
            }

            if (swap) {
                struct proc_info temp = infos[j];
                infos[j] = infos[j + 1];
                infos[j + 1] = temp;
            }
        }
    }
}


// Print a string padded to a fixed width
void print_padded(const char *str, int width) {
    printf("%s", str);
    for (int i = 0; i < width - strlen(str); i++) {
        printf(" ");
    }
}

// void itoa(int num, char *str) {
//     int i = 0;
//     int is_negative = 0;

//     // Handle 0 explicitly
//     if (num == 0) {
//         str[i++] = '0';
//         str[i] = '\0';
//         return;
//     }

//     // Handle negative numbers
//     if (num < 0) {
//         is_negative = 1;
//         num = -num;
//     }

//     // Process digits
//     while (num != 0) {
//         str[i++] = (num % 10) + '0';
//         num /= 10;
//     }

//     // Add negative sign if needed
//     if (is_negative) {
//         str[i++] = '-';
//     }

//     // Null-terminate the string
//     str[i] = '\0';

//     // Reverse the string
//     for (int j = 0, k = i - 1; j < k; j++, k--) {
//         char temp = str[j];
//         str[j] = str[k];
//         str[k] = temp;
//     }
// }


// void utoa(uint num, char *str) {
//     int i = 0;
//     int is_negative = 0;

//     // Handle 0 explicitly
//     if (num == 0) {
//         str[i++] = '0';
//         str[i] = '\0';
//         return;
//     }

//     // Handle negative numbers
//     if (num < 0) {
//         is_negative = 1;
//         num = -num;
//     }

//     // Process digits
//     while (num != 0) {
//         str[i++] = (num % 10) + '0';
//         num /= 10;
//     }

//     // Add negative sign if needed
//     if (is_negative) {
//         str[i++] = '-';
//     }

//     // Null-terminate the string
//     str[i] = '\0';

//     // Reverse the string
//     for (int j = 0, k = i - 1; j < k; j++, k--) {
//         char temp = str[j];
//         str[j] = str[k];
//         str[k] = temp;
//     }
// }


// void lutoa(uint64 num, char *str) {
//     int i = 0;
//     int is_negative = 0;

//     // Handle 0 explicitly
//     if (num == 0) {
//         str[i++] = '0';
//         str[i] = '\0';
//         return;
//     }

//     // Handle negative numbers
//     if (num < 0) {
//         is_negative = 1;
//         num = -num;
//     }

//     // Process digits
//     while (num != 0) {
//         str[i++] = (num % 10) + '0';
//         num /= 10;
//     }

//     // Add negative sign if needed
//     if (is_negative) {
//         str[i++] = '-';
//     }

//     // Null-terminate the string
//     str[i] = '\0';

//     // Reverse the string
//     for (int j = 0, k = i - 1; j < k; j++, k--) {
//         char temp = str[j];
//         str[j] = str[k];
//         str[k] = temp;
//     }
// }
