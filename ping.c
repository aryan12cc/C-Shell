#include "ping.h"
#include "header.h"
#include "background.h"

int ping_string_to_int(char* integer_string) {
    int ans = 0;
    int len = strlen(integer_string);
    for(int i = 0; i < len; i++) {
        if(integer_string[i] < '0' || integer_string[i] > '9') {
            return -2;
        }
        ans = ans * 10 + (integer_string[i] - '0');
    }
    return ans;
}

bool check_process(int pid) {
    for(int i = 0; i < current_background_process_index; i++) {
        if(all_background_processes[i]->pid == pid) {
            return true;
        }
    }
    return false;
}

void ping(char* input, bool print_signal) {
    char* input_copy = (char*) malloc (sizeof(char) * MAX_SIZE);
    char* input_copy_start = input_copy;
    strcpy(input_copy, input);
    char* word;
    int process_pid = -1, signal = -1;
    while((word = strtok_r(input_copy, " ", &input_copy))) {
        if(strcmp(word, "ping") == 0) {
            continue;
        }
        if(process_pid == -1) {
            process_pid = ping_string_to_int(word);
        }
        else if(signal == -1) {
            signal = ping_string_to_int(word);
        }
        else {
            printf("\033[31mERROR: Wrong number of arguments\n\033[0m");
            free(input_copy_start);
            return;
        }
    }
    if(process_pid < 0 || signal < 0) {
        printf("\033[31mERROR: Wrong number of arguments\n\033[0m");
        free(input_copy_start);
        return;
    }
    signal %= 32;
    if(check_process(process_pid) == false) {
        printf("\033[31mERROR: No such process found\n\033[0m");
        free(input_copy_start);
        return;
    }
    if(print_signal == true) {
        printf("Sent signal %d to process with pid %d\n", signal, process_pid);
    }
    if(signal != 18) {
        kill(process_pid, signal);
    }
    else {
        for(int i = 0; i < current_background_process_index; i++) {
            if(all_background_processes[i]->current_state == 2) {
                all_background_processes[i]->current_state = 1;
                kill(all_background_processes[i]->pid, SIGCONT);
            }
        }
    }
    free(input_copy_start);
}