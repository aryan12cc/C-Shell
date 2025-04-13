#include "header.h"
#include "display.h"
#include "commands.h"
#include "log.h"
#include "background.h"

void init() {
    tcgetattr(STDIN_FILENO, &actual_terminal);
    log_execute = false;
    current_background = false;
    current_command_name = (char*) malloc (sizeof(char) * MAX_SIZE);
    terminal_pid = getpid();
    terminal_pgid = getpgid(terminal_pid);
    for(int i = 0; i < MAX_PROCESSES; i++) {
        all_background_processes[i] = (struct background_process*) malloc (sizeof(struct background_process));
        all_background_processes[i]->command_name = (char*) malloc (sizeof(char) * MAX_SIZE);
    }
    current_background_process_index = 0;
    getcwd(current_directory, MAX_SIZE);
    getcwd(home_directory, MAX_SIZE);
    strcpy(previous_directory, current_directory);
    strcpy(log_file_path, home_directory);
    strcat(log_file_path, "/log.txt");
    prompt_required = true;
    for(int i = 0; i < MAX_SIZE; i++) {
        extra_display_prompt[i] = '\0';
    }
}

void clear_background_processes() {
    free(current_command_name);
    for(int i = 0; i < MAX_PROCESSES; i++) {
        free(all_background_processes[i]->command_name);
        free(all_background_processes[i]);
    }
}

void read_myshrc() {
    FILE* f = fopen(".myshrc", "r");
    if(f == NULL) {
        printf("\033[31mERROR: Can't open .myshrc file\n\033[0m");
        return;
    }
    char line[MAX_SIZE];
    alias_pointer = 0;
    function_pointer = 0;
    char* new_line_pointer = (char*) malloc (sizeof(char) * MAX_SIZE);
    char* new_line_pointer_start = new_line_pointer;
    bool inside_function = false;
    while(fgets(line, MAX_SIZE, f) != NULL) {
        if(strncmp(line, "alias", 5) == 0) {
            new_line_pointer = &line[5];
            char* command = (char*) malloc (sizeof(char) * MAX_SIZE);
            char* command_start = command;
            command = strtok_r(new_line_pointer, "=", &new_line_pointer);
            strcpy(alias_commands[alias_pointer][0], command);
            command = strtok_r(new_line_pointer, "=", &new_line_pointer);
            strcpy(alias_commands[alias_pointer][1], command);
            delete_spaces(alias_commands[alias_pointer][0]);
            delete_spaces(alias_commands[alias_pointer][1]);
            free(command_start);
            alias_pointer += 1;
        }
        delete_spaces(line);
        if(line[strlen(line) - 1] == ')' && line[strlen(line) - 2] == '(') {
            inside_function = true;
            strcpy(function_commands[function_pointer][0], line);
            int len = strlen(function_commands[function_pointer][0]);
            function_commands[function_pointer][0][len - 1] = '\0';
            function_commands[function_pointer][0][len - 2] = '\0';
            continue;
        }
        if(strcmp(line, "}") == 0) {
            inside_function = false;
            function_pointer += 1;
        }
        if(inside_function == true && strcmp(line, "{") != 0) {
            int current_pointer = strlen(function_commands[function_pointer][1]);
            for(int i = 0; i < strlen(line); i++) {
                if(i + 3 < strlen(line) && strncmp(&line[i], "\"$1\"", 4) == 0) {
                    function_commands[function_pointer][1][current_pointer] = '@';
                    current_pointer += 1;
                    i += 4;
                }
                else {
                    function_commands[function_pointer][1][current_pointer] = line[i];
                    current_pointer += 1;
                }
            }
            function_commands[function_pointer][1][current_pointer] = ';';
            current_pointer += 1;
        }
    }
    free(new_line_pointer_start);
    fclose(f);
}

int main() {
    handle_sigchild(SIGCHLD);
    init();
    read_myshrc();
    char next_command[MAX_SIZE * 2];
    char* input = (char*) malloc (sizeof(char) * MAX_SIZE);
    char* input_copy = (char*) malloc (sizeof(char) * MAX_SIZE);
    signal(SIGCHLD, handle_sigchild);
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    is_previous_command_valid = false;
    while(true) {
        contains_log_command = false;
        display_prompt();
        prompt_required = true;
        for(int i = 0; i < MAX_SIZE; i++) {
            extra_display_prompt[i] = '\0';
        }
        update_background_processes();
        if(fgets(input, MAX_SIZE, stdin) == NULL) { // Ctrl + D
            printf("\n"); // to ensure we go the new line
            free(input_copy);
            free(input);
            clear_background_processes();
            return 0;
        }
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
                is_previous_command_valid = true;
                process_command(next_command);
                if(current_background == false && is_previous_command_valid == true) {
                    end_time = time(0);
                    int total_time = (end_time - start_time);
                    if(total_time >= 2) {
                        add_to_prompt(current_command_name, total_time);
                    }
                }
            current_background = false;
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
                clear_background_processes();
                return 0;
            }
            is_previous_command_valid = true;
            process_command(next_command);
            if(current_background == false && is_previous_command_valid == true) {
                end_time = time(0);
                int total_time = (end_time - start_time);
                if(total_time >= 2) {
                    add_to_prompt(current_command_name, total_time);
                }
            }
            current_background = false;
        }
        if(!contains_log_command) {
            add_command_to_log(input_copy);
        }
    }
    return 0;
}