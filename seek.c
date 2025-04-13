#include "seek.h"
#include "header.h"
#include "hop.h"

bool flag_d, flag_e, flag_f;
char last_found_path[MAX_SIZE];
int total_found_count;

bool check_flags(char* input) {
    if(input[0] != '-' || strlen(input) == 1) {
        return false;
    }
    bool current_flag_d = false, current_flag_e = false, current_flag_f = false;
    for(int i = 1; i < strlen(input); i++) {
        if(input[i] == 'd') {
            current_flag_d = true;
        }
        else if(input[i] == 'e') {
            current_flag_e = true;
        }
        else if(input[i] == 'f') {
            current_flag_f = true;
        }
        else {
            return false;
        }
    }
    flag_d = (flag_d || current_flag_d);
    flag_e = (flag_e || current_flag_e);
    flag_f = (flag_f || current_flag_f);
    return true;
}

void get_directory(char* directory_path, char* command) {
    if(strcmp(command, "~") == 0) {
        strcpy(directory_path, home_directory);
    }
    else if(strcmp(command, ".") == 0) {
        strcpy(directory_path, current_directory);
    }
    else if(strcmp(command, "..") == 0) {
        strcpy(directory_path, current_directory);
        for(int i = strlen(current_directory) - 1; i >= 0; i--) {
            if(directory_path[i] == '/') {
                directory_path[i] = '\0';
                break;
            }
            directory_path[i] = '\0';
        }
    }
    else if(strcmp(command, "-") == 0) {
        strcpy(directory_path, previous_directory);
    }
    else {
        strcpy(directory_path, command);
    }
}

bool check_directory(char* path) {
    // printf("come here!\n");
    struct stat buffer;
    stat(path, &buffer);
    // printf("omk\n");
    if(S_ISDIR(buffer.st_mode)) {
        // printf("tm\n");
        return true;
    }
    return false;
}

bool check_executable(char* path) {
    struct stat buffer;
    stat(path, &buffer);
    if(buffer.st_mode & S_IXUSR) {
        return true;
    }
    return false;
}

bool compare_names(char* str1, char* str2) {
    if(strlen(str2) < strlen(str1)) {
        return false;
    }
    for(int i = 0; i < strlen(str1); i++) {
        if(str1[i] != str2[i]) {
            return false;
        }
    }
    return true;
}

void check_directories_recursive(char* directory_path, char* relative_directory, char* check_prefix) {
    // printf("the following are the values:\n");
    // printf("directory path = %s\n", directory_path);
    // printf("relative directory = %s\n", relative_directory);
    // printf("prefix = %s\n", check_prefix);
    struct dirent* one_entry;
    DIR* directory_entries;
    directory_entries = opendir(directory_path);
    char* file_list[MAX_SIZE];
    int index = 0;
    while((one_entry = readdir(directory_entries)) != NULL) {
        file_list[index] = (char*) malloc (sizeof(char) * MAX_SIZE);
        strcpy(file_list[index], directory_path);
        // printf("directory path = %s\n", directory_path);
        if(directory_path[strlen(directory_path) - 1] != '/') {
            strcat(file_list[index], "/");
        }
        char* file_name = (char*) malloc (sizeof(char) * MAX_SIZE);
        strcpy(file_name, one_entry->d_name);
        strcat(file_list[index++], file_name);
        // printf("index = %d\n", index);
        // printf("file name = %s\n", file_name);
        if(compare_names(check_prefix, file_name) == true) {
            // printf("compare_names(true)\n");
            if(check_directory(file_list[index - 1]) && flag_f == false) {
                strcpy(last_found_path, ".");
                strcat(last_found_path, &file_list[index - 1][strlen(relative_directory)]);
                printf("\033[34m%s\n\033[0m", last_found_path);
                strcpy(last_found_path, "hop ");
                strcat(last_found_path, file_list[index - 1]);
                total_found_count += 1;
            }
            else if(!check_directory(file_list[index - 1]) && !check_executable(file_list[index - 1]) && flag_d == false) {
                strcpy(last_found_path, ".");
                strcat(last_found_path, &file_list[index - 1][strlen(relative_directory)]);
                printf("\033[32m%s\n\033[0m", last_found_path);
                strcpy(last_found_path, file_list[index - 1]);
                total_found_count += 1;
            }
        }
        // printf("exited compare names\n");
        if(check_directory(file_list[index - 1]) && strcmp(file_name, ".") != 0 && strcmp(file_name, "..") != 0) {
            // printf("check directories recursive!\n");
            // printf("relative directory = %s\n", relative_directory);
            // printf("check_prefix = %s\n", check_prefix);
            // printf("file list = %s\n", file_list[index - 1]);
            check_directories_recursive(file_list[index - 1], relative_directory, check_prefix);
        }
        // printf("come here!\n");
        free(file_name);
        // printf("wrong free lmao L!\n");
    }
    for(int i = 0; i < index; i++) {
        free(file_list[i]);
    }
    closedir(directory_entries);
}

void seek(char* input) {
    flag_d = false;
    flag_e = false;
    flag_f = false;
    total_found_count = 0;
    char* input_copy = (char*) malloc (sizeof(char) * MAX_SIZE);
    strcpy(input_copy, input);
    char* input_copy_start = input_copy;
    char* command = (char*) malloc (sizeof(char) * MAX_SIZE);
    char* command_start = command;
    command = strtok_r(input_copy, " ", &input_copy);
    int current_stage_of_input = 1; // 1 - flags, 2 - word seek, 3 - directory path
    char* word_seek = (char*) malloc (sizeof(char) * MAX_SIZE);
    char* directory_path = (char*) malloc (sizeof(char) * MAX_SIZE);
    for(int i = 0; i < MAX_SIZE; i++) {
        directory_path[i] = '\0';
    }
    while((command = strtok_r(input_copy, " ", &input_copy)) != NULL) {
        if(current_stage_of_input == 1) {
            if(!check_flags(command)) {
                current_stage_of_input = 2;
            }
        }
        if(current_stage_of_input == 2) {
            strcpy(word_seek, command);
            current_stage_of_input = 3;
        }
        else if(current_stage_of_input == 3) {
            get_directory(directory_path, command);
        }
    }
    if(strcmp(directory_path, "") == 0) {
        strcpy(directory_path, current_directory);
    }
    if(directory_path[0] == '~' && strlen(directory_path) != 1) {
        char* new_directory_path = (char*) malloc (sizeof(char) * MAX_SIZE);
        strcpy(new_directory_path, home_directory);
        strcat(new_directory_path, &directory_path[1]);
        strcpy(directory_path, new_directory_path);
        free(new_directory_path);
    }
    if(flag_d && flag_f) {
        printf("\033[31mERROR: Invalid flags!\n\033[0m");
        free(input_copy_start);
        free(command_start);
        free(word_seek);
        free(directory_path);
        return;
    }
    if(!check_directory(directory_path)) {
        printf("\033[31mERROR: The given directory does not exist\n\033[0m");
        free(input_copy_start);
        free(command_start);
        free(word_seek);
        free(directory_path);
        return;
    }
    check_directories_recursive(directory_path, directory_path, word_seek);
    if(total_found_count == 0) {
        printf("No match found!\n");
    }
    if(total_found_count == 1 && flag_e == true) {
        if(strncmp(last_found_path, "hop", 3) == 0 && check_directory(&last_found_path[4]) == true) {
            hop(last_found_path);
        }
        else {
            FILE *f = fopen(last_found_path, "r");
            if(f == NULL) {
                printf("\033[31mERROR: Missing permissions!\n\033[0m");
                free(input_copy_start);
                free(command_start);
                free(word_seek);
                free(directory_path);
            }
            char line[MAX_SIZE];
            while(fgets(line, MAX_SIZE, f) != NULL) {
                printf("%s", line);
            }
            printf("\n");
            fclose(f);
        }
    }
    free(input_copy_start);
    free(command_start);
    free(word_seek);
    free(directory_path);
}