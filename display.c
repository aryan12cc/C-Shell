#include "header.h"
#include "display.h"

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