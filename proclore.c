#include "header.h"
#include "proclore.h"

void proclore_int_to_string(int pid, char* str) {
    int str_index = 0;
    while(pid != 0) {
        char c = (char)('0' + (pid % 10));
        pid /= 10;
        str[str_index] = c;
        str_index += 1;
    }
    for(int i = 0; i < str_index / 2; i++) {
        char c = str[i];
        str[i] = str[str_index - 1 - i];
        str[str_index - 1 - i] = c;
    }
    str[str_index] = '\0';
}

int proclore_string_to_int(char* str) {
    int ans = 0;
    for(int i = 0; i < strlen(str); i++) {
        ans *= 10;
        ans += str[i] - '0';
    }
    return ans;
}

void proclore(char* input) {
    char* input_copy = (char*) malloc (sizeof(char) * MAX_SIZE);
    char* input_copy_start = input_copy;
    strcpy(input_copy, input);
    if(strcmp(input_copy, "proclore") == 0) { // print process of your own terminal
        strcat(input_copy, " ");
        char* string_pid = (char*) malloc (sizeof(char) * 20);
        proclore_int_to_string((int)getpid(), string_pid);
        strcat(input_copy, string_pid);
        free(string_pid);
    }
    char* command = (char*) malloc (sizeof(char) * MAX_SIZE);
    char* command_start = command;
    command = strtok_r(input_copy, " ", &input_copy);
    command = strtok_r(input_copy, " ", &input_copy);
    if((strtok_r(input_copy, " ", &input_copy)) != NULL) {
        printf("\033[31mERROR: More than one arguments given to proclore\n\033[0m");
        free(input_copy_start);
        free(command_start);
        return;
    }
    char* file_path = (char*) malloc (sizeof(char) * MAX_SIZE);
    strcpy(file_path, "/proc/");
    strcat(file_path, command); // command contains the pid
    strcat(file_path, "/status");
    FILE* file = fopen(file_path, "r");
    if(file == NULL) {
        printf("\033[31mERROR: Cannot open file. Possibly because PID doesn't exist\n\033[0m");
        free(input_copy_start);
        free(command_start);
        free(file_path);
        return;
    }
    char line[MAX_SIZE];
    int counter = 0;
    printf("pid : %s\n", command);
    int pid = proclore_string_to_int(command);
    while(fgets(line, MAX_SIZE, file) != NULL) {
        counter += 1;
        if(counter == 3) {
            printf("process status : %c", line[7]);
            int current_foreground_process_group = tcgetpgrp(STDOUT_FILENO);
            int process_group = getpgid(pid);
            if(current_foreground_process_group == process_group) {
                printf("+");
            }
            printf("\n");
            printf("Process group : %d\n", process_group);
        }
        if(counter == 19) {
            printf("Virtual memory : %s", &line[12]);
        }
    }
    char* exec_path = (char*) malloc (sizeof(char) * MAX_SIZE);
    strcpy(file_path, "/proc/");
    strcat(file_path, command);
    strcat(file_path, "/exe");
    int len = readlink(file_path, exec_path, MAX_SIZE - 1);
    exec_path[len] = '\0';
    printf("executable path : %s\n", exec_path);
    
    free(exec_path);
    free(input_copy_start);
    free(command_start);
    free(file_path);
    fclose(file);
}