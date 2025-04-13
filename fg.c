#include "header.h"
#include "fg.h"
#include "ping.h"

void put_job_in_foreground(int pid_num) {
    signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
    if(tcsetpgrp(STDIN_FILENO, getpgid(pid_num)) == -1) {
        return;
    }
    kill(pid_num, SIGCONT);
    int status;
    waitpid(pid_num, &status, WUNTRACED);
    tcsetpgrp(STDIN_FILENO, terminal_pgid);
    signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
    change_process_to_stopped(pid_num);
    // remove_background_process(pid_num);
}

void fg(char* input) {
    int pid_num = ping_string_to_int(&input[3]);
    if(pid_num == -2 || check_process(pid_num) == false) {
        printf("\033[31mERROR: No such process found\n\033[0m");
        return;
    }
    put_job_in_foreground(pid_num);
}