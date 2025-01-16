#include "header.h"
#include "display.h"
#include "commands.h"
#include "log.h"
#include "background.h"

int main() {
    char next_command[MAX_SIZE * 2];
    getcwd(current_directory, MAX_SIZE);
    getcwd(home_directory, MAX_SIZE);
    strcpy(previous_directory, current_directory);
    strcpy(log_file_path, home_directory);
    strcat(log_file_path, "/log.txt");
    prompt_required = true;
    for(int i = 0; i < MAX_SIZE; i++) {
        extra_display_prompt[i] = '\0';
    }
    char* input = (char*) malloc (sizeof(char) * MAX_SIZE);
    char* input_copy = (char*) malloc (sizeof(char) * MAX_SIZE);
    while(true) {
        contains_log_command = false;
        display_prompt();
        prompt_required = true;
        for(int i = 0; i < MAX_SIZE; i++) {
            extra_display_prompt[i] = '\0';
        }
        fgets(input, MAX_SIZE, stdin);
        input[strlen(input) - 1] = '\0'; // truncating endline
        strcpy(input_copy, input);
        // account for random spaces
        int current_index = 0;
        delete_spaces(input);
        separate_commands(input);
        int next_command_index = 0;
        for(int i = 0; i < strlen(input); i++) {
            if(input[i] == ';') {
                next_command[next_command_index] = '\0';
                if(strcmp(next_command, "exit") == 0) {
                    free(input_copy);
                    free(input);
                }
                process_command(next_command);
                next_command_index = 0;
            }
            else {
                next_command[next_command_index++] = input[i];
            }
        }
        if(next_command_index != 0) {
            next_command[next_command_index] = '\0';
            if(strcmp(next_command, "exit") == 0) {
                free(input_copy);
                free(input);
            }
            process_command(next_command);
        }
        if(!contains_log_command) {
            add_command_to_log(input_copy);
        }
    }
    return 0;
}