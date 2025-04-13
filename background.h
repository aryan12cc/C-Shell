#ifndef BACKGROUND_H
#define BACKGROUND_H

#define MAX_PROCESSES 4100 // max number of pid's

typedef struct background_process {
    char* command_name;
    int current_state;
    pid_t pid;
    /*
    current_state:
    0 --> terminated / not initialized
    1 --> running
    2 --> stopped
    */
} background_process;

struct background_process* all_background_processes[MAX_PROCESSES];
int current_background_process_index;

void handle_sigcont(int signal);

void run_background_process(char* command);

#endif