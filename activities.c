#include "header.h"
#include "activities.h"
#include "background.h"

void print_activities() {
    char* current_status = (char*) malloc (sizeof(char) * 20);
    for(int i = 0; i < current_background_process_index; i++) {
        if(all_background_processes[i]->current_state == 1) {
            strcpy(current_status, "Running");
        }
        else if(all_background_processes[i]->current_state == 2) {
            strcpy(current_status, "Stopped");
        }
        printf("%d : %s - %s\n", all_background_processes[i]->pid, all_background_processes[i]->command_name, current_status);
    }
    free(current_status);
}