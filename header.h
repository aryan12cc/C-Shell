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

#include <sys/utsname.h>
#include <sys/stat.h>
#include <dirent.h>

// global variables
#define MAX_SIZE 4100
char current_directory[MAX_SIZE];
char previous_directory[MAX_SIZE];
char home_directory[MAX_SIZE];
char extra_display_prompt[MAX_SIZE];

bool contains_log_command;

#endif