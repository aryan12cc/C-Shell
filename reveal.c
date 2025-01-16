#include "header.h"
#include "reveal.h"

bool flag_a, flag_l;

void get_flags(char* word) {
    for(int i = 1; i < strlen(word); i++) {
        if(word[i] == 'a') {
            flag_a = true;
        }
        else if(word[i] == 'l') {
            flag_l = true;
        }
        else {
            printf("\033[31mERROR: Could not identify the flag '%c'\n\033[0m", word[i]);
        }
    }
}

int cmp(const void* a, const void* b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

void generate_file_path(char* input, char* file_name, char* file_path) {
    for(int i = 0; i < MAX_SIZE; i++) {
        file_path[i] = '\0';
    }
    strcat(file_path, input);
    if(input[strlen(input) - 1] != '/') {
        strcat(file_path, "/");
    }
    strcat(file_path, file_name);
}

int directory_check(char* file_path) {
    struct stat buffer;
    stat(file_path, &buffer);
    if(S_ISDIR(buffer.st_mode)) {
        return 1;
    }
    return 0;
}

int executable_check(char* file_path) {
    struct stat buffer;
    stat(file_path, &buffer);
    if(buffer.st_mode & S_IXUSR) {
        return 1;
    }
    return 0;
}

void check_file_type(struct stat buffer) {
    if(S_ISDIR(buffer.st_mode)) {
        printf("d");
    }
    else if(S_ISLNK(buffer.st_mode)) {
        printf("l");
    }
    else if(S_ISCHR(buffer.st_mode)) {
        printf("c");
    }
    else if(S_ISBLK(buffer.st_mode)) {
        printf("b");
    }
    else if(S_ISFIFO(buffer.st_mode)) {
        printf("p");
    }
    else {
        printf("-");
    }
}

void permissions(struct stat buffer) {
    printf("%c", (S_IRUSR & buffer.st_mode) ? ('r') : ('-'));
    printf("%c", (S_IWUSR & buffer.st_mode) ? ('w') : ('-'));
    printf("%c", (S_IXUSR & buffer.st_mode) ? ('x') : ('-'));
    printf("%c", (S_IRGRP & buffer.st_mode) ? ('r') : ('-'));
    printf("%c", (S_IWGRP & buffer.st_mode) ? ('w') : ('-'));
    printf("%c", (S_IXGRP & buffer.st_mode) ? ('x') : ('-'));
    printf("%c", (S_IROTH & buffer.st_mode) ? ('r') : ('-'));
    printf("%c", (S_IWOTH & buffer.st_mode) ? ('w') : ('-'));
    printf("%c", (S_IXOTH & buffer.st_mode) ? ('x') : ('-'));
    printf(" ");
}

void format_time(struct stat buffer, char* last_modification) {
    // remove day
    for(int i = 4; i < strlen(last_modification); i++) {
        last_modification[i - 4] = last_modification[i];
    }
    last_modification[strlen(last_modification) - 4] = '\0';
    // remove seconds
    for(int i = 15; i < strlen(last_modification); i++) {
        last_modification[i - 3] = last_modification[i];
    }
    last_modification[strlen(last_modification) - 3] = '\0';
    time_t current_time = time(NULL);
    double diff_time = difftime(current_time, buffer.st_mtime);
    if(diff_time <= 60 * 60 * 24 * 30 * 6) {
        // remove year
        last_modification[13] = '\0';
    }
    else {
        // remove time
        for(int i = 13; i < 17; i++) {
            last_modification[i - 6] = last_modification[i];
        }
        last_modification[strlen(last_modification) - 6] = '\0';
    }
}

void generate_output_no_l_flag(char* file_path, char* file_list) {
    if(directory_check(file_path) == true) {
        printf("\033[34m%s\n\033[0m", file_list);
    }
    else if(executable_check(file_path) == true) {
        printf("\033[32m%s\n\033[0m", file_list);
    }
    else {
        printf("%s\n", file_list);
    }
}

void generate_output_l_flag(char* file_path, char* file_list) {
    struct stat buffer;
    stat(file_path, &buffer);
    bool change_color = false;
    if(directory_check(file_path) == true) {
        printf("\033[34m");
        change_color = true;
    }
    else if(executable_check(file_path) == true) {
        printf("\033[32m");
        change_color = true;
    }
    check_file_type(buffer);
    permissions(buffer);
    printf("%hu ", buffer.st_nlink);
    struct passwd* user_details = getpwuid(buffer.st_uid);
    printf("%s ", user_details->pw_name);
    struct group* group_details = getgrgid(buffer.st_gid);
    printf("%s ", group_details->gr_name);
    printf("%lld ", buffer.st_size);
    char* last_modification = (char*) malloc (sizeof(char) * MAX_SIZE);
    strcpy(last_modification, ctime(&buffer.st_ctime));
    format_time(buffer, last_modification);
    last_modification[strlen(last_modification) - 1] = '\0';
    printf("%s ", last_modification);
    printf("%s", file_list);
    if(change_color == true) {
        printf("\033[0m");
    }
    printf("\n");
    free(last_modification);
}

void print_list(char* command) {
    char* command_copy = (char*) malloc (sizeof(char) * MAX_SIZE);
    strcpy(command_copy, command);
    char* command_copy_start = command_copy;
    struct dirent* one_entry;
    DIR* directory_entries;
    if(strcmp(command_copy, "-") == 0) {
        strcpy(command_copy, previous_directory);
    }
    if(strcmp(command_copy, "..") == 0) {
        strcpy(command_copy, "./..");
    }
    if(command_copy[0] == '~') {
        char* temp = (char*) malloc (sizeof(char) * MAX_SIZE);
        strcpy(temp, &command_copy[1]);
        strcpy(command_copy, home_directory);
        strcat(command_copy, temp);
        free(temp);
    }
    struct stat check_directory;
    stat(command_copy, &check_directory);
    if(!S_ISDIR(check_directory.st_mode)) {
        if(access(command_copy, F_OK) == 0) {
            char* temporary_file_name = (char*) malloc (sizeof(char) * MAX_SIZE);
            char* temporary_file_name_start = temporary_file_name;
            strcpy(temporary_file_name, command_copy);
            struct stat buffer;
            stat(temporary_file_name, &buffer);
            int total_size = buffer.st_size;
            char* file_name = (char*) malloc (sizeof(char) * MAX_SIZE);
            char* file_name_start = file_name;
            int cnt = 0;
            for(int i = 0; i < strlen(temporary_file_name); i++) {
                if(temporary_file_name[i] == '/') {
                    cnt += 1;
                }
            }
            while(cnt != 0) {
                file_name = strtok_r(temporary_file_name, "/", &temporary_file_name);
                cnt--;
            }
            if(flag_l == 0) {
                generate_output_no_l_flag(command_copy, temporary_file_name);
            }
            else {
                printf("total %d\n", total_size);
                generate_output_l_flag(command_copy, temporary_file_name);
            }
            free(file_name_start);
            free(temporary_file_name_start);
            free(command_copy_start);
        }
        else {
            printf("\033[31mERROR: File does not exist\n\033[0m");
        }
        return;
    }
    directory_entries = opendir(command_copy);
    if(directory_entries == NULL) {
        printf("\033[31mERROR: Could not open directory\n\033[0m");
    }
    else {
        char* file_list[MAX_SIZE];
        int index = 0;
        while((one_entry = readdir(directory_entries)) != NULL) {
            if(flag_a == 0 && one_entry->d_name[0] == '.') {
                continue;
            }
            file_list[index] = (char*) malloc (sizeof(char) * MAX_SIZE);
            strcpy(file_list[index++], one_entry->d_name);
        }
        qsort(file_list, index, sizeof(char*), cmp);
        if(flag_l == 0) {
            for(int i = 0; i < index; i++) {
                char* file_path = (char*) malloc (sizeof(char) * MAX_SIZE);
                generate_file_path(command_copy, file_list[i], file_path);
                generate_output_no_l_flag(file_path, file_list[i]);
                free(file_path);
            }
        }
        else {
            // total value at the top
            // file description, hard links, owner name, group name, file size, time stamp, file name
            int total_size = 0;
            for(int i = 0; i < index; i++) {
                char* file_path = (char*) malloc (sizeof(char) * MAX_SIZE);
                generate_file_path(command_copy, file_list[i], file_path);
                struct stat buffer;
                stat(file_path, &buffer);
                total_size += buffer.st_size;
                free(file_path);
            }
            printf("total %d\n", total_size);
            for(int i = 0; i < index; i++) {
                char* file_path = (char*) malloc (sizeof(char) * MAX_SIZE);
                generate_file_path(command_copy, file_list[i], file_path);
                generate_output_l_flag(file_path, file_list[i]);
                free(file_path);
            }
        }
        for(int i = 0; i < index; i++) {
            free(file_list[i]);
        }
        closedir(directory_entries);
    }
    free(command_copy_start);
}

void reveal(char* input) {
    flag_a = false;
    flag_l = false;
    char* input_copy = (char*) malloc (sizeof(char) * MAX_SIZE);
    char* input_copy_start = input_copy;
    strcpy(input_copy, input);
    input_copy[strlen(input)] = '\0';
    char* command = (char*) malloc (sizeof(char) * MAX_SIZE);
    char* command_start = command;
    command = strtok_r(input_copy, " ", &input_copy); // reveal command
    bool argument = false;
    // rest of the commands
    while(true) {
        command = strtok_r(input_copy, " ", &input_copy);
        if(command == NULL) {
            break;
        }
        if(command[0] == '-' && strlen(command) != 1) { // ensure that reveal - shows the last directory reveal
            get_flags(command);
        }
        else {
            argument = true;
            print_list(command);
        }
    }
    if(argument == false) {
        print_list(".");
    }
    free(command_start);
    free(input_copy_start);
}