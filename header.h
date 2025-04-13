#ifndef HEADER_H
#define HEADER_H

// standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

// global variables
#define MAX_SIZE 4100
#define MAX_ALIAS 260
#define MAX_ALIAS_SIZE 260
char current_directory[MAX_SIZE];
char previous_directory[MAX_SIZE];
char home_directory[MAX_SIZE];
char extra_display_prompt[MAX_SIZE];
char alias_commands[MAX_ALIAS][2][MAX_ALIAS_SIZE];
char function_commands[MAX_ALIAS][2][MAX_ALIAS_SIZE];
char* current_command_name;

int terminal_pid;
int terminal_pgid;
bool contains_log_command;
bool is_previous_command_valid;
int alias_pointer;
int function_pointer;

struct termios actual_terminal;

time_t start_time, end_time;
bool log_execute, current_background;

#endif