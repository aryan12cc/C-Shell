#include "header.h"
#include "background.h"
#include "display.h"

void change_process_to_stopped(pid_t id) {
    for(int i = 0; i < current_background_process_index; i++) {
        if(all_background_processes[i]->pid == id) {
            all_background_processes[i]->current_state = 2;
        }
    }
}

void add_background_process(pid_t id, char* command_name, int state) {
    all_background_processes[current_background_process_index]->pid = id;
    all_background_processes[current_background_process_index]->current_state = state;
    strcpy(all_background_processes[current_background_process_index]->command_name, command_name);
    current_background_process_index += 1;
}

void remove_background_process(pid_t id) {
    for(int i = 0; i < current_background_process_index; i++) {
        if(all_background_processes[i]->pid == id) {
            for(int j = i + 1; j < current_background_process_index; j++) {
                all_background_processes[j - 1]->pid = all_background_processes[j]->pid;
                all_background_processes[j - 1]->current_state = all_background_processes[j]->current_state;
                strcpy(all_background_processes[j - 1]->command_name, all_background_processes[j]->command_name);
            }
            current_background_process_index -= 1;
            break;
        }
    }
}

void update_background_processes() {
    char path[MAX_SIZE];
    for(int i = 0; i < current_background_process_index; i++) {
        snprintf(path, sizeof(path), "/proc/%d/", all_background_processes[i]->pid);
        if(access(path, F_OK) != 0) {
            for(int j = i + 1; j < current_background_process_index; j++) {
                all_background_processes[j - 1]->pid = all_background_processes[j]->pid;
                all_background_processes[j - 1]->current_state = all_background_processes[j]->current_state;
                strcpy(all_background_processes[j - 1]->command_name, all_background_processes[j]->command_name);
            }
            current_background_process_index -= 1;
            i -= 1;
        }
    }
}

void handle_sigchild(int signal) {
    pid_t pid;
    int status;
    while((pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED)) > 0) {
        bool stopped = true;
        for(int i = 0; i < current_background_process_index; i++) {
            if(all_background_processes[i]->pid == pid) {
                if(WIFEXITED(status)) {
                    printf("\n%s exited normally (%d)\n", all_background_processes[i]->command_name, all_background_processes[i]->pid);
                    stopped = false;
                }
                else if(WIFSTOPPED(status)) {
                    all_background_processes[i]->current_state = 2;
                }
                else if(WIFCONTINUED(status)) {
                    all_background_processes[i]->current_state = 1;
                }
                else if(WIFSIGNALED(status)) {
                    printf("\n%s exited abnormally (%d)\n", all_background_processes[i]->command_name, all_background_processes[i]->pid);
                    stopped = false;
                }
            }
        }
        if(stopped == false) {
            remove_background_process(pid);
        }
    }
}

void run_background_process(char* command) {
    char* exec_args[MAX_SIZE];
    char* temp_command = (char*) malloc (sizeof(char) * MAX_SIZE);
    char* temp_command_start = temp_command;
    char* word;
    int exec_args_cnt = 0;
    strcpy(temp_command, command);
    while((word = strtok_r(temp_command, " ", &temp_command)) != NULL) {
        if(strcmp(word, "&") == 0) {
            break;
        }
        exec_args[exec_args_cnt] = (char*) malloc (sizeof(char) * MAX_SIZE);
        strcpy(exec_args[exec_args_cnt], word);
        exec_args_cnt += 1;
    }
    exec_args[exec_args_cnt] = NULL;
    free(temp_command_start);
    int current_id = getpid();
    pid_t child_id = fork();
    struct sigaction sa;
    sa.sa_handler = handle_sigchild;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if(sigaction(SIGCHLD, &sa, NULL) == -1) {
        printf("\033[31mERROR: Sigaction error\n\033[0m");
        exit(0);
    }
    if(child_id < 0) {
        printf("\033[31mERROR: Can't run the process (error in creating child)\n\033[0m");
        for(int i = 0; i < exec_args_cnt; i++) {
            free(exec_args[i]);
        }
    }
    else if(child_id == 0) {
        setpgid(0, 0);
        setpgrp();
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        printf("%d\n", getpid());
        if(execvp(exec_args[0], exec_args) < 0) {
            printf("\033[31mERROR: Can't execute the given command: %s\n\033[0m", exec_args[0]);
            for(int i = 0; i < exec_args_cnt; i++) {
                free(exec_args[i]);
            }
            exit(0);
        }
    }
    else {
        setpgid(child_id, child_id);
        add_background_process(child_id, exec_args[0], 1);
        for(int i = 0; i < exec_args_cnt; i++) {
            free(exec_args[i]);
        }
        signal(SIGCHLD, handle_sigchild);
    }
}