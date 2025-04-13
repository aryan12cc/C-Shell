#include "header.h"
#include "bg.h"
#include "ping.h"

void bg(char* input) {
    char* ping_input = (char*) malloc (sizeof(char) * MAX_SIZE);
    strcpy(ping_input, "ping");
    strcat(ping_input, &input[2]);
    strcat(ping_input, " 18");
    ping(ping_input, false);
    free(ping_input);
}