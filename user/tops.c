#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/sysinfo_data.h"

// Sorting fields
#define SORT_BY_PID 0
#define SORT_BY_VRUNTIME 1
#define SORT_BY_WEIGHT 2

// Helper Functions
void print_padded(const char *str, int width);
void fetch_and_display(int sort_field);
void sort_procs(struct proc_info *infos, int count, int sort_field);

void itoa(int num, char *str);
void utoa(uint num, char *str);
void lutoa(uint64 num, char *str);

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

    char input[2];
    while (1) {
        if (async_read(input, 2) > 0) {
            if (input[0] == 'q') {
                printf("\nExiting visualizer...\n");
                exit(0);
            }
            if (input[0] == 'v') {
                sort_field = SORT_BY_VRUNTIME;
            }
            if (input[0] == 'w') {
                sort_field = SORT_BY_WEIGHT;
            }
            if (input[0] == 'p') {
                sort_field = SORT_BY_PID;
            }
        }

        fetch_and_display(sort_field);
        sleep(10);
    }
    return 0;
}

// Fetch process information and display it
void fetch_and_display(int sort_field) {
    struct proc_info infos[64]; // Buffer for proc_info structs
    int count = sysinfo(infos); // Fetch system-wide process info using sysinfo()
    char str[16];

    if (count < 0) {
        printf("Failed to fetch process information.\n");
        return;
    }

    // Sort processes based on the selected field
    sort_procs(infos, count, sort_field);

    // Clear the screen
    printf("\033[H\033[J"); // Clear the screen
    printf("=== Process Information ===\n\n");

    // Print table headers
    print_padded(sort_field == SORT_BY_PID ? "PID*" : "PID", 5);
    print_padded("Name", 10);
    print_padded(sort_field == SORT_BY_WEIGHT ? "Weight*" : "Weight", 8);
    print_padded("State", 10);
    print_padded(sort_field == SORT_BY_VRUNTIME ? "VRT*" : "VRT", 10);
    print_padded("ART", 10);
    print_padded("LT", 10);
    printf("\n");
    for (int i = 0; i < 71; i++) printf("-"); // Horizontal separator
    printf("\n");
    
    // Print process information
    for (int i = 0; i < count; i++) {
        itoa(infos[i].pid, str);
        print_padded(str, 5);

        print_padded(infos[i].name, 10);
        
        itoa(infos[i].weight, str);
        print_padded(str, 8);

        print_padded(infos[i].state, 10);
        
        lutoa(infos[i].vruntime, str);
        print_padded(str, 10);

        lutoa(infos[i].runtime, str);
        print_padded(str, 10);

        lutoa(infos[i].lifetime, str);
        print_padded(str, 10);
        
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
            } else if (sort_field == SORT_BY_WEIGHT && infos[j].weight < infos[j + 1].weight) {
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


// Print a string padded to a fixed width
void print_padded(const char *str, int width) {
    printf("%s", str);
    for (int i = 0; i < width - strlen(str); i++) {
        printf(" ");
    }
}

void itoa(int num, char *str) {
    int i = 0;
    int is_negative = 0;

    // Handle 0 explicitly
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    // Handle negative numbers
    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    // Process digits
    while (num != 0) {
        str[i++] = (num % 10) + '0';
        num /= 10;
    }

    // Add negative sign if needed
    if (is_negative) {
        str[i++] = '-';
    }

    // Null-terminate the string
    str[i] = '\0';

    // Reverse the string
    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}


void utoa(uint num, char *str) {
    int i = 0;
    int is_negative = 0;

    // Handle 0 explicitly
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    // Handle negative numbers
    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    // Process digits
    while (num != 0) {
        str[i++] = (num % 10) + '0';
        num /= 10;
    }

    // Add negative sign if needed
    if (is_negative) {
        str[i++] = '-';
    }

    // Null-terminate the string
    str[i] = '\0';

    // Reverse the string
    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}


void lutoa(uint64 num, char *str) {
    int i = 0;
    int is_negative = 0;

    // Handle 0 explicitly
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    // Handle negative numbers
    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    // Process digits
    while (num != 0) {
        str[i++] = (num % 10) + '0';
        num /= 10;
    }

    // Add negative sign if needed
    if (is_negative) {
        str[i++] = '-';
    }

    // Null-terminate the string
    str[i] = '\0';

    // Reverse the string
    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}
