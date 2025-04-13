#ifndef PING_H
#define PING_H

#include "header.h"

int ping_string_to_int(char* integer_string);
bool check_process(int pid);
void ping(char* input, bool print_signal);

#endif