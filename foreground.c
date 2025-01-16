#include "header.h"
#include "foreground.h"

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
    time_t start_time = time(0);
    int current_id = getpid();
    int child_id = fork();
    if(child_id < 0) {
        printf("\033[31mERROR: Can't run the process (error in creating child)\n\033[0m");
        for(int i = 0; i < exec_args_cnt; i++) {
            free(exec_args[i]);
        }
    }
    else if(child_id == 0) {
        exec_args[exec_args_cnt] = '\0';
        if(execvp(exec_args[0], exec_args) < 0) {
            printf("\033[31mERROR: Can't execute the given command: %s\n\033[0m", command);
            exit(0);
        }
    }
    else {
        int stat;
        waitpid(child_id, &stat, 0);
        for(int i = 0; i < exec_args_cnt; i++) {
            free(exec_args[i]);
        }
        time_t end_time = time(0);
        int time_difference = difftime(end_time, start_time);
        if(time_difference >= 2) {
            int extra_display_prompt_length = strlen(extra_display_prompt);
            if(strlen(extra_display_prompt) != 0) {
                strcat(extra_display_prompt, ";");
            }
            else {
                strcat(extra_display_prompt, " ");
            }
            extra_display_prompt_length += 1;
            char* first_command = (char*) malloc (sizeof(char) * MAX_SIZE);
            char* first_command_start = first_command;
            first_command = strtok_r(command, " ", &command);
            strcat(extra_display_prompt, first_command);
            extra_display_prompt_length += strlen(first_command);
            free(first_command_start);
            strcat(extra_display_prompt, " : ");
            extra_display_prompt_length += 3;
            char* time_difference_string = (char*) malloc (sizeof(char) * 20);
            for(int i = 0; i < 20; i++) {
                time_difference_string[i] = '\0';
            }
            foreground_int_to_string(time_difference, time_difference_string);
            extra_display_prompt_length += strlen(time_difference_string);
            strcat(extra_display_prompt, time_difference_string);
            extra_display_prompt[extra_display_prompt_length] = '\0';
            free(time_difference_string);
        }
    }
}