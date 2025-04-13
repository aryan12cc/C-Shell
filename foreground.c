#include "header.h"
#include "foreground.h"
#include "background.h"

void foreground_int_to_string(int time_taken, char* str) {
    while(time_taken != 0) {
        char digit = (time_taken % 10) + '0';
        time_taken /= 10;
        str[strlen(str)] = digit;
    }
    for(int i = 0; i < strlen(str) / 2; i++) {
        char c = str[i];
        str[i] = str[strlen(str) - 1 - i];
        str[strlen(str) - 1 - i] = c;
    }
    str[strlen(str)] = 's';
    str[strlen(str)] = '\0';
}

void run_foreground_process(char* command) {
    char* exec_args[MAX_SIZE];
    char* temp_command = (char*) malloc (sizeof(char) * MAX_SIZE);
    char* temp_command_start = temp_command;
    char* word = (char*) malloc (sizeof(char) * MAX_SIZE);
    char* word_start = word;
    int exec_args_cnt = 0;
    strcpy(temp_command, command);
    while((word = strtok_r(temp_command, " ", &temp_command)) != NULL) {
        exec_args[exec_args_cnt] = (char*) malloc (sizeof(char) * MAX_SIZE);
        strcpy(exec_args[exec_args_cnt], word);
        exec_args_cnt += 1;
    }
    free(temp_command_start);
    free(word_start);
    int current_id = getpid();
    int child_id = fork();
    if(child_id < 0) {
        printf("\033[31mERROR: Can't run the process (error in creating child)\n\033[0m");
        for(int i = 0; i < exec_args_cnt; i++) {
            free(exec_args[i]);
        }
    }
    else if(child_id == 0) {
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        exec_args[exec_args_cnt] = '\0';
        if(execvp(exec_args[0], exec_args) < 0) {
            printf("\033[31mERROR: Can't execute the given command: %s\n\033[0m", command);
            exit(0);
        }
    }
    else {
        setpgid(child_id, 0);
        signal(SIGTTOU, SIG_IGN);
        tcsetpgrp(STDIN_FILENO, child_id);
        int stat;
        waitpid(child_id, &stat, WUNTRACED);
        tcsetpgrp(STDIN_FILENO, getpgrp());
        if(WIFSTOPPED(stat)) {
            add_background_process(child_id, exec_args[0], 2);
        }
        for(int i = 0; i < exec_args_cnt; i++) {
            free(exec_args[i]);
        }
    }
}