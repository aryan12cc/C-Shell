#include "man.h"
#include "header.h"

void fetch_page(char* input) {
    struct hostent* server;
    struct sockaddr_in server_address;
    char request[MAX_SIZE], response[MAX_SIZE];

    char* host = (char*) malloc (sizeof(char) * MAX_SIZE);
    strcpy(host, "man.he.net");

    char* page = (char*) malloc (sizeof(char) * MAX_SIZE);
    strcpy(page, "/man1/");
    strcat(page, input);

    server = gethostbyname(host);
    if(server == NULL) {
        printf("\033[31mERROR: Cannot reach the host\n\033[0m");
        return;
    }
    int ret = socket(AF_INET, SOCK_STREAM, 0);
    if(ret < 0) {
        printf("\033[31mERROR: Cannot open socket\n\033[0m");
        return;
    }
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(80);
    bcopy((char*) server->h_addr, (char*) &server_address.sin_addr.s_addr, server->h_length);

    if(connect(ret, (struct sockaddr*) &server_address, sizeof(server_address)) < 0) {
        printf("\033[31mERROR: Cannot connect to man page\n\033[0m");
        close(ret);
        return;
    }

    strcpy(request, "GET ");
    strcat(request, page);
    strcat(request, " HTTP/1.1\r\nHost: ");
    strcat(request, host);
    strcat(request, "\r\nConnection: close\r\n\r\n");

    if(write(ret, request, strlen(request)) < 0) {
        printf("\033[31mERROR: Cannot write to socket\n\033[0m");
        close(ret);
        return;
    }
    int received;
    bool header_end = false;
    while((received = read(ret, response, sizeof(response) - 1)) > 0) {
        response[received] = '\0';
        if(header_end == false) {
            char* header_end_idx = strstr(response, "<");
            if(header_end_idx != NULL) {
                header_end = true;
                bool check = true;
                for(int i = 0; i < strlen(header_end_idx); i++) {
                    if(header_end_idx[i] == '<') {
                        check = false;
                    }
                    if(check) printf("%c", header_end_idx[i]);
                    if(header_end_idx[i] == '>') {
                        check = true;
                    }
                }
            }
        }
        else {
            bool check = true;
            for(int i = 0; i < strlen(response); i++) {
                if(response[i] == '<') {
                    check = false;
                }
                if(check) printf("%c", response[i]);
                if(response[i] == '>') {
                    check = true;
                }
            }
        }
    }
    if(received < 0) {
        printf("\033[31mERROR: Cannot read from socket\n\033[0m");
        close(ret);
        return;
    }
    close(ret);
    free(host);
    free(page);
}

void fetch_man_page(char* input) {
    char* input_copy = (char*) malloc (sizeof(char) * MAX_SIZE);
    char* input_copy_start = input_copy;
    strcpy(input_copy, input);
    input_copy[strlen(input)] = '\0';
    char* command = (char*) malloc (sizeof(char) * MAX_SIZE);
    char* command_start = command;
    command = strtok_r(input_copy, " ", &input_copy);
    while(true) {
        command = strtok_r(input_copy, " ", &input_copy);
        break;
    }
    // iman command
    fetch_page(command);
    free(input_copy_start);
    free(command_start);
}