#ifndef HTTPS
#define HTTPS

#include <stdio.h>
#include <stdlib.h>

struct req {
    char* path;
    char* method;
    struct settings* settings;
};

struct res {
    unsigned int status;
    FILE* body;
};

struct settings {
    char* docRoot;
    char* index;
};

int create_server(char* port);
void server_listen(int sockfd, int queue, void (*handle)(struct req*, struct res*), struct settings* settings);
void send_response(FILE* clientfile, struct res* res);
int parse_req_line(char* line, struct req* req);
struct req* parse_req(FILE* client);

#endif
