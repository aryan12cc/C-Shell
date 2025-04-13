#include "header.h"
#include "display.h"
#include "foreground.h"

void add_to_prompt(char* command, int time_difference) {
    int extra_display_prompt_length = strlen(extra_display_prompt);
    if(strlen(extra_display_prompt) != 0) {
        strcat(extra_display_prompt, ";");
    }
    else {
        strcat(extra_display_prompt, " ");
    }
    extra_display_prompt_length += 1;
    strcat(extra_display_prompt, command);
    extra_display_prompt_length += strlen(command);
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

void display_prompt() {
    char system_name[MAX_SIZE];
    uid_t username_uid = getuid();
    struct passwd* shell_details = getpwuid(username_uid);
    // username
    char* username = shell_details->pw_name;
    //system name
    gethostname(system_name, MAX_SIZE);

    bool is_under_home_directory = true;
    for(int i = 0; i < strlen(home_directory); i++) {
        if(i == strlen(current_directory) || home_directory[i] != current_directory[i]) {
            is_under_home_directory = false;
            break;
        }
    }
    if(!is_under_home_directory) {
        if(prompt_required) {
            printf("<%s@%s:%s%s> ", username, system_name, current_directory, extra_display_prompt);
        }
    }
    else {
        char* directory_name = (char*) malloc (sizeof(char) * MAX_SIZE);
        strcpy(directory_name, home_directory);
        directory_name[0] = '~';
        int current_index = 1;
        for(int i = strlen(home_directory); i < strlen(current_directory); i++) {
            directory_name[current_index] = current_directory[i];
            current_index += 1;
        }
        directory_name[current_index] = '\0';
        if(prompt_required) {
            printf("<%s@%s:%s%s> ", username, system_name, directory_name, extra_display_prompt);
        }
        free(directory_name);
    }
}