#include "header.h"
#include "commands.h"
#include "display.h"
#include "hop.h"
#include "reveal.h"
#include "log.h"
#include "background.h"
#include "foreground.h"

void delete_spaces(char* input) {
    int input_size = strlen(input);
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
    if(input[input_size - 1] == ' ') {
        input[input_size - 1] = '\0';
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

void process_command(char* input) {
    // remove spaces if any
    if(strlen(input) == 0) {
        return;
    }
    if(input[0] == ' ') {
        for(int i = 1; i < strlen(input); i++) {
            input[i - 1] = input[i];
        }
        input[strlen(input) - 1] = '\0';
    }
    // remove '\n' if any
    if(input[strlen(input) - 1] == '\n') {
        input[strlen(input) - 1] = '\0';
    }
    char input_copy[strlen(input)];
    strcpy(input_copy, input);
    char* command = strtok(input_copy, " ");
    if(strcmp(command, "hop") == 0) {
        hop(input);
    }
    else if(strcmp(command, "reveal") == 0) {
        reveal(input);
    }
    else if(strcmp(command, "log") == 0) {
        contains_log_command = true;
        get_log(input);
    }
    else if(strcmp(command, "proclore") == 0) {
        proclore(input);
    }
    else if(strcmp(command, "seek") == 0) {
        seek(input);
    }
    // else if(strcmp(command, "exit") == 0) {
    //     exit(0);
    // }
    else {
        // foreground / background process
        if(input[strlen(input) - 1] == '&') {
            run_background_process(input);
        }
        else {
            run_foreground_process(input);
        }
    }
}