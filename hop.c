#include "header.h"
#include "hop.h"

void hop(char* input) {
    bool errorneous_input = false;
    char* input_copy = (char*) malloc (sizeof(char) * MAX_SIZE);
    char* input_copy_start = input_copy;
    strcpy(input_copy, input);
    if(strcmp(input_copy, "hop") == 0) {
        // similar to hop ~
        strcat(input_copy, " ~");
    }
    char* command = (char*) malloc (sizeof(char) * MAX_SIZE);
    char* command_start = command;
    command = strtok_r(input_copy, " ", &input_copy); // hop command
    // rest of the commands
    while((command = strtok_r(input_copy, " ", &input_copy))) {
        if(strcmp(command, ".") == 0) {
            // stay in current directory
            strcpy(previous_directory, current_directory);
        }
        else if(strcmp(command, "..") == 0) {
            // go to parent directory
            int ret_value = chdir("..");
            if(ret_value == 0) {
                strcpy(previous_directory, current_directory);
                getcwd(current_directory, MAX_SIZE);
            }
            else {
                printf("\033[31mERROR: Can't go to parent directory\n\033[0m");
            }
        }
        else if(command[0] == '~') {
            char* new_directory_path = (char*) malloc (sizeof(char) * MAX_SIZE);
            strcpy(new_directory_path, home_directory);
            if(strlen(command) != 1) {
                strcat(new_directory_path, &command[1]);
            }
            int ret_value = chdir(new_directory_path);
            if(ret_value == 0) {
                strcpy(previous_directory, current_directory);
                getcwd(current_directory, MAX_SIZE);
            }
            else {
                printf("\033[31mERROR: Can't go to home directory\n\033[0m");
            }
            free(new_directory_path);
        }
        else if(strcmp(command, "-") == 0) {
            // go to previous directory
            int ret_value = chdir(previous_directory);
            char previous_directory_copy[strlen(previous_directory)];
            strcpy(previous_directory_copy, previous_directory);
            strcpy(previous_directory, current_directory);
            strcpy(current_directory, previous_directory_copy);
        }
        else {
            int ret_value = chdir(command);
            if(ret_value == 0) {
                strcpy(previous_directory, current_directory);
                getcwd(current_directory, MAX_SIZE);
            }
            else {
                errorneous_input = true;
                printf("\033[31mERROR: Can't go to the specified directory\n\033[0m");
            }
        }
        printf("%s\n", current_directory);
        if(!errorneous_input) {
            char current_command[MAX_SIZE];
            strcpy(current_command, "hop ");
            strcat(current_command, command);
        }
    }
    free(command_start);
    free(input_copy_start);
}