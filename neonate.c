#include "header.h"
#include "neonate.h"

// https://stackoverflow.com/questions/11987495/what-do-the-numbers-in-proc-loadavg-mean-on-linux

int neonate_string_to_int(char* string) {
    int ans = 0;
    if(strlen(string) >= 5) {
        printf("\033[31mCannot stop for more than 99999 seconds\n\033[0m");
        return -2;
    }
    for(int i = 0; i < strlen(string); i++) {
        if(string[i] < '0' || string[i] > '9') {
            return -1;
        }
        ans = (ans * 10) + (string[i] - '0');
    }
    return ans;
}

void print_latest_process(int num) {
    struct termios neonate_terminal = actual_terminal;
    neonate_terminal.c_lflag &= ~(ICANON | ECHO);
    neonate_terminal.c_cc[VMIN] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &neonate_terminal);
    time_t neonate_start, neonate_current;
    int current_multiplier = 1;
    neonate_start = time(0);
    // double d_num = (double)(num);
    // if(d_num == 0) {
    //     d_num = 0.001;
    // }
    char* path = (char*) malloc (sizeof(char) * MAX_SIZE);
    for(int i = 0; i < MAX_SIZE; i++) {
        path[i] = '\0';
    }
    strcpy(path, "/proc/loadavg");
    while(true) {
        neonate_current = time(0);
        if(neonate_start + current_multiplier * num <= neonate_current) {
            current_multiplier += 1;
            FILE* f = fopen(path, "r");
            if(f == NULL) {
               printf("\033[31mERROR: Cannot open the loadavg file\n\033[0m");
               break;
            }
            char line[MAX_SIZE];
            fgets(line, MAX_SIZE, f);
            int space_cnt = 0, idx = 0;
            for(int i = 0; i < strlen(line); i++) {
                if(line[i] == ' ') {
                    space_cnt += 1;
                    if(space_cnt == 4) {
                        idx = i + 1;
                        break;
                    }
                }
            }
            int process_id = 0;
            for(int i = idx; i < strlen(line) - 1; i++) {
                process_id = process_id * 10 + line[i] - '0';
            }
            printf("%d\n", process_id);
            fclose(f);
        }
        char ch = 'a';
        if(read(STDIN_FILENO, &ch, 1) == -1) {
            printf("\033[31mERROR: Cannot read from stdin\n\033[0m");
            break;
        }
        if(ch == 'x') {
            break;
        }
    }
    free(path);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &actual_terminal);
}

void neonate(char* input) {
    char* input_copy = (char*) malloc (sizeof(char) * MAX_SIZE);
    char* input_copy_start = input_copy;
    strcpy(input_copy, input);
    input_copy[strlen(input)] = '\0';
    char* command = (char*) malloc (sizeof(char) * MAX_SIZE);
    char* command_start = command;
    command = strtok_r(input_copy, " ", &input_copy);
    int current_argument = 2;
    int num_seconds = -1;
    while(true) {
        command = strtok_r(input_copy, " ", &input_copy);
        if(command == NULL) {
            break;
        }
        if(current_argument == 2) {
            if(strcmp(command, "-n") != 0) {
                printf("\033[31mERROR: Invalid parameters of neonate\n\033[0m");
                return;
            }
        }
        else {
            num_seconds = neonate_string_to_int(command);
        }
        current_argument += 1;
    }
    if(num_seconds == -1 || current_argument != 4) {
        printf("\033[31mERROR: Invalid parameters of neonate\n\033[0m");
        return;
    }
    if(num_seconds == -2) {
        return;
    }
    print_latest_process(num_seconds);
    free(input_copy_start);
    free(command_start);
}