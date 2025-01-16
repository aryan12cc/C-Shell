#include "header.h"
#include "log.h"
#include "commands.h"

void add_command_to_log(char* command) {
    // it is a valid command always
    for(int i = strlen(command); i < MAX_SIZE; i++) {
        command[i] = '\0';
    }
    strcat(command, "\n");
    FILE* file_pointer = fopen(log_file_path, "r");
    if(file_pointer == NULL) {
        printf("\033[31mERROR: Log cannot be cleared\n\033[0m");
        return;
    }
    char all_lines[15][MAX_SIZE];
    char line[MAX_SIZE];
    int cnt = 0;
    while(fgets(line, MAX_SIZE, file_pointer) != NULL) {
        strcpy(all_lines[cnt], line);
        cnt += 1;
    }
    fclose(file_pointer);
    file_pointer = fopen(log_file_path, "w");
    if(file_pointer == NULL) {
        printf("\033[31mERROR: Log cannot be cleared\n\033[0m");
        return;
    }
    if(cnt != 0 && (cnt != 15 || strcmp(all_lines[cnt - 1], command) == 0)) {
        fprintf(file_pointer, "%s", all_lines[0]);
    }
    for(int i = 1; i < cnt; i++) {
        fprintf(file_pointer, "%s", all_lines[i]);
    }
    if(cnt == 0 || strcmp(all_lines[cnt - 1], command) != 0) {
        fprintf(file_pointer, "%s", command);
    }
    fclose(file_pointer);
}

void execute_command(int command_index) {
    FILE* file_pointer = fopen(log_file_path, "r");
    if(file_pointer == NULL) {
        printf("\033[31mERROR: Log cannot be cleared\n\033[0m");
        return;
    }
    int cnt = 0;
    char line[MAX_SIZE];
    while(fgets(line, MAX_SIZE, file_pointer) != NULL) {
        cnt += 1;
    }
    fclose(file_pointer);
    int command_to_execute = cnt - command_index;
    if(command_to_execute < 0) {
        printf("\033[31mERROR: Invalid operation - There are lesser than %d commands in the log\n\033[0m", command_index);
        return;
    }
    cnt = 0;
    file_pointer = fopen(log_file_path, "r");
    if(file_pointer == NULL) {
        printf("\033[31mERROR: Log cannot be cleared\n\033[0m");
        return;
    }
    while(fgets(line, MAX_SIZE, file_pointer) != NULL) {
        if(cnt == command_to_execute) {
            fclose(file_pointer);
            process_command(line);
            break;
        }
        cnt += 1;
    }
}

void show_log() {
    FILE* file_pointer = fopen(log_file_path, "r");
    if(file_pointer == NULL) {
        printf("\033[31mERROR: Log cannot be cleared\n\033[0m");
        return;
    }
    char line[MAX_SIZE];
    while(fgets(line, MAX_SIZE, file_pointer) != NULL) {
        printf("%s", line);
    }
    fclose(file_pointer);
}

void purge_log() {
    // clears the contents of the file
    FILE* file_pointer = fopen(log_file_path, "w");
    if(file_pointer == NULL) {
        printf("\033[31mERROR: Log cannot be cleared\n\033[0m");
        return;
    }
    fclose(file_pointer);
}

int string_to_int(char* command) {
    int ans = 0;
    if(strlen(command) > 8) {
        return -1;
    }
    for(int i = 0; i < strlen(command); i++) {
        if(command[i] < '0' || command[i] > '9') {
            return -1;
        }
        ans = (ans * 10) + (command[i] - '0');
    }
    return ans;
}

void get_log(char* input) {
    char* input_copy = (char*) malloc (sizeof(char) * MAX_SIZE);
    char* input_copy_start = input_copy;
    strcpy(input_copy, input);
    if(strcmp(input_copy, "log") == 0) {
        show_log();
        free(input_copy_start);
        return;
    }
    else if(strcmp(input_copy, "log purge") == 0) {
        purge_log();
        free(input_copy_start);
        return;
    }
    char* command = (char*) malloc (sizeof(char) * MAX_SIZE);
    char* command_start = command;
    command = strtok_r(input_copy, " ", &input_copy); // log command
    int flag = 0;
    // rest of the commands
    while((command = strtok_r(input_copy, " ", &input_copy))) {
        if(strcmp(command, "execute") != 0 && flag == 0) {
            flag = 2;
            printf("\033[31mERROR: Invalid command entered\n\033[0m");
        }
        else if(strcmp(command, "execute") == 0 && flag == 0) {
            flag = 1;
        }
        else if(flag == 1) {
            int index = string_to_int(command);
            if(index == -1) {
                printf("\033[31mERROR: Invalid command entered\033[0m");
            }
            else {
                execute_command(index);
            }
            flag = 2;
        }
    }
    free(input_copy_start);
    free(command_start);
}