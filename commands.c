#include "header.h"
#include "commands.h"
#include "display.h"
#include "hop.h"
#include "reveal.h"
#include "log.h"
#include "background.h"
#include "foreground.h"
#include "proclore.h"
#include "seek.h"
#include "activities.h"
#include "ping.h"
#include "fg.h"
#include "bg.h"
#include "neonate.h"
#include "man.h"

void delete_spaces(char* input) {
    int input_size = strlen(input);
    for(int i = 0; i < input_size; i++) {
        if(input[i] == '\n') {
            input[i] = ' ';
        }
    }
    // change '\t' to ' '
    for(int i = 0; i < input_size; i++) {
        if(input[i] == '\t') {
            input[i] = ' ';
        }
    }
    // remove trailing spaces
    int current_index = 0;
    while(current_index != input_size && input[current_index] == ' ') {
        current_index += 1;
    }
    for(int i = 0; i < input_size - current_index; i++) {
        input[i] = input[i + current_index];
    }
    input[input_size - current_index] = '\0';
    input_size = strlen(input);
    // remove extra spaces
    current_index = 0;
    for(int i = 1; i < input_size; i++) {
        if(input[i] == ' ' && input[current_index] == ' ') {
            continue;
        }
        input[++current_index] = input[i];
    }
    input[current_index + 1] = '\0';
    input_size = strlen(input);
    // check if last index is a space or not
    while(input[input_size - 1] == ' ') {
        input[input_size - 1] = '\0';
        input_size -= 1;
    }
}

void separate_commands(char* input) {
    // add ; after every &
    char new_input[MAX_SIZE * 2];
    int new_input_index = 0;
    for(int i = 0; i < strlen(input); i++) {
        if(input[i] == '&') {
            new_input[new_input_index++] = ' ';
            new_input[new_input_index++] = '&';
            new_input[new_input_index++] = ';';
        }
        else {
            new_input[new_input_index++] = input[i];
        }
    }
    new_input[new_input_index] = '\0';
    // ensure there is a single ; after every command
    new_input_index = 1;
    char last_non_space = new_input[0];
    for(int i = 1; i < strlen(new_input); i++) {
        if(new_input[i] == ';' && last_non_space == ';') {
            continue;
        }
        new_input[new_input_index++] = new_input[i];
        if(new_input[i] != ' ') {
            last_non_space = new_input[i];
        }
    }
    new_input[new_input_index] = '\0';
    strcpy(input, new_input);
}

int get_input_file(char* input_file, char* input) {
    for(int i = 0; i < MAX_SIZE; i++) {
        input_file[i] = '\0';
    }
    int input_file_size = 0;
    for(int j = 0; j < strlen(input); j++) {
        if(input[j] == ' ' && input_file_size == 0) {
            continue;
        }
        if(input[j] == ' ' || input[j] == '>' || input[j] == '&') {
            return j;
        }
        input_file[input_file_size] = input[j];
        input_file_size += 1;
    }
    return strlen(input);
}

int get_output_file(char* output_file, char* output) {
    for(int i = 0; i < MAX_SIZE; i++) {
        output_file[i] = '\0';
    }
    int output_file_size = 0;
    for(int j = 0; j < strlen(output); j++) {
        if(output[j] == ' ' && output_file_size == 0) {
            continue;
        }
        if(output[j] == ' ' || output[j] == '>' || output[j] == '&') {
            return j;
        }
        output_file[output_file_size] = output[j];
        output_file_size += 1;
    }
    return strlen(output);
}

void check_alias(char* command) {
    int command_len = strlen(command);
    for(int i = 0; i < alias_pointer; i++) {
        int alias_len = strlen(alias_commands[i][0]);
        if(command_len >= alias_len && strncmp(command, alias_commands[i][0], alias_len) == 0) {
            char* new_command = (char*) malloc (sizeof(char) * MAX_SIZE);
            strcpy(new_command, alias_commands[i][1]);
            int new_command_length = strlen(new_command);
            for(int i = alias_len; i < command_len; i++) {
                new_command[new_command_length] = command[i];
                new_command_length += 1;
            }
            new_command[new_command_length] = '\0';
            strcpy(command, new_command);
            free(new_command);
            check_alias(command);
            break;
        }
    }
}

void check_functions(char* command) {
    char* command_copy = (char*) malloc (sizeof(char) * MAX_SIZE);
    strcpy(command_copy, command);
    char* command_copy_start = command_copy;
    char* word = strtok_r(command_copy, " ", &command_copy);
    if(word == NULL) {
        return;
    }
    word = (char*) malloc (sizeof(char) * MAX_SIZE);
    strcpy(word, command_copy);
    int command_len = strlen(command);
    strcpy(command_copy, command);
    char* new_command = (char*) malloc (sizeof(char) * MAX_SIZE);
    char* new_command_start = new_command;
    char* new_new_command = (char*) malloc (sizeof(char) * MAX_SIZE);
    char* new_new_command_start = new_new_command;
    for(int i = 0; i < function_pointer; i++) {
        int function_len = strlen(function_commands[i][0]);
        if(command_len >= function_len && strncmp(command, function_commands[i][0], function_len) == 0) {
            strcpy(new_command, function_commands[i][1]);
            int new_command_length = strlen(new_command);
            int new_new_command_length = 0;
            for(int j = 0; j < new_command_length; j++) {
                if(new_command[j] == '@') {
                    for(int k = 0; k < strlen(word); k++) {
                        new_new_command[new_new_command_length] = word[k];
                        new_new_command_length += 1;
                    }
                }
                else if(new_command[j] == ';') {
                    new_new_command[new_new_command_length] = '\0';
                    process_command(new_new_command);
                    for(int k = 0; k < new_new_command_length; k++) {
                        new_new_command[k] = '\0';
                    }
                    new_new_command_length = 0;
                }
                else {
                    new_new_command[new_new_command_length] = new_command[j];
                    new_new_command_length += 1;
                }
            }
            break;
        }
    }
    free(command_copy_start);
    free(new_new_command_start);
    free(new_command_start);
}

void check_which_command(char* command) {
    delete_spaces(command);
    check_alias(command);
    check_functions(command);
    char* command_copy = (char*) malloc (sizeof(char) * MAX_SIZE);
    strcpy(command_copy, command);
    char* command_copy_start = command_copy;
    char* first_word = strtok_r(command_copy, " ", &command_copy);
    strcpy(current_command_name, first_word);
    start_time = time(0);
    if(strcmp(first_word, "hop") == 0) {
        hop(command);
    }
    else if(strcmp(first_word, "reveal") == 0) {
        reveal(command);
    }
    else if(strcmp(first_word, "log") == 0) {
        contains_log_command = true;
        get_log(command);
    }
    else if(strcmp(first_word, "proclore") == 0) {
        proclore(command);
    }
    else if(strcmp(first_word, "seek") == 0) {
        seek(command);
    }
    else if(strcmp(first_word, "activities") == 0) {
        print_activities();
    }
    else if(strcmp(first_word, "ping") == 0) {
        ping(command, true);
    }
    else if(strcmp(first_word, "bg") == 0) {
        bg(command);
    }
    else if(strcmp(first_word, "fg") == 0) {
        fg(command);
    }
    else if(strcmp(first_word, "neonate") == 0) {
        neonate(command);
    }
    else if(strcmp(first_word, "iMan") == 0) {
        fetch_man_page(command);
    }
    else if(command[strlen(command) - 1] == '&') {
        int command_len = strlen(command);
        for(int i = 0; i < function_pointer; i++) {
            int function_len = strlen(function_commands[i][0]);
            if(command_len >= function_len && strncmp(command, function_commands[i][0], function_len) == 0) {
                return;
            }
        }
        current_background = true;
        run_background_process(command);
    }
    else {
        int command_len = strlen(command);
        for(int i = 0; i < function_pointer; i++) {
            int function_len = strlen(function_commands[i][0]);
            if(command_len >= function_len && strncmp(command, function_commands[i][0], function_len) == 0) {
                return;
            }
        }
        run_foreground_process(command);
    }
    free(command_copy_start);
}

void get_command_name(char* output_string, char* command) {
    delete_spaces(command);
    char* command_copy = (char*) malloc (sizeof(char) * MAX_SIZE);
    strcpy(command_copy, command);
    char* command_copy_start = command_copy;
    char* first_word = strtok_r(command_copy, " ", &command_copy);
    strcpy(output_string, first_word);
    free(command_copy_start);
    return;
}

bool check_first_word_is_log(char* command) {
    char* command_copy = (char*) malloc (sizeof(char) * MAX_SIZE);
    strcpy(command_copy, command);
    char* command_copy_start = command_copy;
    char* first_word = strtok_r(command_copy, " ", &command_copy);
    if(strcmp(first_word, "log") == 0) {
        free(command_copy_start);
        return true;
    }
    free(command_copy_start);
    return false;
}

void check_end_time(time_t start_time, char* command) {
    end_time = time(0);
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
        char* command_copy = (char*) malloc (sizeof(char) * MAX_SIZE);
        char* command_copy_start = command_copy;
        strcpy(command_copy, command);
        char* first_command = (char*) malloc (sizeof(char) * MAX_SIZE);
        char* first_command_start = first_command;
        first_command = strtok_r(command_copy, " ", &command_copy);
        strcat(extra_display_prompt, first_command);
        extra_display_prompt_length += strlen(first_command);
        free(first_command_start);
        free(command_copy_start);
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

void check_pipes_and_io_redirection(char* input) {
    if(input[0] == '|' || input[strlen(input) - 1] == '|') {
        is_previous_command_valid = false;
        printf("\033[31mERROR: Invalid usage of pipes - Begin / End with pipe\n\033[0m");
        return;
    }
    char* pipe_separated_commands[MAX_SIZE];
    int pipe_separated_commands_index = 0;
    char* single_command = strtok_r(input, "|", &input);
    while(single_command != NULL) {
        pipe_separated_commands[pipe_separated_commands_index] = (char*) malloc (sizeof(char) * MAX_SIZE);
        strcpy(pipe_separated_commands[pipe_separated_commands_index], single_command);
        delete_spaces(pipe_separated_commands[pipe_separated_commands_index]);
        if(strlen(pipe_separated_commands[pipe_separated_commands_index]) == 0) {
            // consecutive pipes -- invalid
            is_previous_command_valid = false;
            printf("\033[31mERROR: Invalid usage of pipes - Consecutive pipes\n\033[0m");
            return;
        }
        pipe_separated_commands_index += 1;
        single_command = strtok_r(input, "|", &input);
    }
    int pipes[pipe_separated_commands_index - 1][2];
    for(int i = 0; i < pipe_separated_commands_index - 1; i++) {
        pipe(pipes[i]);
    }
    for(int i = 0; i < pipe_separated_commands_index; i++) {
        delete_spaces(pipe_separated_commands[i]);
    }
    int old_in = dup(STDIN_FILENO);
    int old_out = dup(STDOUT_FILENO);
    bool input_redirect = false, output_truncate_redirect = false, output_append_redirect = false;
    char* input_file, *output_file;
    char* new_command = (char*) malloc (sizeof(char) * MAX_SIZE);
    int fd_in = -1, fd_out = -1;
    for(int i = 0; i < pipe_separated_commands_index; i++) {
        input_redirect = false;
        output_truncate_redirect = false;
        output_append_redirect = false;
        for(int j = 0; j < MAX_SIZE; j++) {
            new_command[j] = '\0';
        }
        int new_command_index = 0;
        bool everything_ok = true;
        for(int j = 0; j < strlen(pipe_separated_commands[i]); j++) {
            if(pipe_separated_commands[i][j] == '<') {
                input_redirect = true;
                input_file = (char*) malloc (sizeof(char) * MAX_SIZE);
                j += get_input_file(input_file, &pipe_separated_commands[i][j + 1]);
                int input_file_size = strlen(input_file);
                if(input_file_size == 0) {
                    is_previous_command_valid = false;
                    printf("\033[31mERROR: Invalid input file\n\033[0m");
                    break;
                }
            }
            else if(pipe_separated_commands[i][j] == '>') {
                output_file = (char*) malloc (sizeof(char) * MAX_SIZE);
                if(j != strlen(pipe_separated_commands[i]) && pipe_separated_commands[i][j + 1] == '>') {
                    output_append_redirect = true;
                    j += get_output_file(output_file, &pipe_separated_commands[i][j + 2]) + 1;
                }
                else {
                    output_truncate_redirect = true;
                    j += get_output_file(output_file, &pipe_separated_commands[i][j + 1]);
                }
                int output_file_size = strlen(output_file);
                if(output_file_size == 0) {
                    is_previous_command_valid = false;
                    printf("\033[31mERROR: Invalid output file\n\033[0m");
                    break;
                }
            }
            else {
                new_command[new_command_index] = pipe_separated_commands[i][j];
                new_command_index += 1;
            }
        }
        if(input_redirect == true) {
            if(access(input_file, F_OK) == -1) {
                is_previous_command_valid = false;
                printf("\033[31mERROR: Cannot open input file\n\033[0m");
                break;
            }
            fd_in = open(input_file, O_RDONLY);
            if(fd_in < 0) {
                is_previous_command_valid = false;
                printf("\033[31mERROR: Cannot open input file\n\033[0m");
                break;
            }
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }
        if(output_truncate_redirect == true) {
            fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if(fd_out < 0) {
                is_previous_command_valid = false;
                printf("\033[31mERROR: Cannot open output file\n\033[0m");
                break;
            }
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }
        else if(output_append_redirect == true) {
            fd_out = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if(fd_out < 0) {
                is_previous_command_valid = false;
                printf("\033[31mERROR: Cannot open output file\n\033[0m");
                break;
            }
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }
        if(pipe_separated_commands_index != 1) {
            start_time = time(0);
            int pid = fork();
            if(pid == 0) {
                if(new_command[new_command_index - 1] == '&') {
                    if(i != pipe_separated_commands_index - 1) {
                        is_previous_command_valid = false;
                        printf("\033[31mERROR: '&' has to be at the end of the whole command\n\033[0m");
                    }
                    exit(0);
                }
                if(i > 0) {
                    dup2(pipes[i - 1][0], STDIN_FILENO);
                }
                if(i < pipe_separated_commands_index - 1) {
                    dup2(pipes[i][1], STDOUT_FILENO);
                }
                delete_spaces(new_command);
                check_which_command(new_command);
                exit(0);
            }
            else {
                int status;
                waitpid(pid, &status, 0);
                if(WIFEXITED(status)) {
                    int exit_status = WEXITSTATUS(status);
                    if(exit_status == 1) {
                        break;
                    }
                }
                end_time = time(0);
                if(new_command[new_command_index - 1] == '&') {
                    if(i > 0) {
                        dup2(pipes[i - 1][0], STDIN_FILENO);
                    }
                    run_background_process(new_command);
                }
                get_command_name(current_command_name, new_command);
                if(check_first_word_is_log(new_command) == true) {
                    contains_log_command = true;
                }
                if(i > 0) {
                    close(pipes[i - 1][0]);
                }
                if(i < pipe_separated_commands_index - 1) {
                    close(pipes[i][1]);
                }
                check_end_time(start_time, new_command);
            }
        }
        else {
            check_which_command(new_command);
        }
    }
    dup2(old_in, STDIN_FILENO);
    dup2(old_out, STDOUT_FILENO);
    close(old_in);
    close(old_out);
    free(new_command);
    for(int i = 0; i < pipe_separated_commands_index; i++) {
        free(pipe_separated_commands[i]);
    }
}  

void process_command(char* input) {
    if(strlen(input) == 0) {
        return;
    }
    delete_spaces(input);
    check_pipes_and_io_redirection(input);
}