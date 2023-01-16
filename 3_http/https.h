/**
 * @file https.h
 * @author Lorenz Hörburger 12024737
 * @brief HTTP Server API
 *
 * @version 0.1
 * @date 15.01.2023
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef HTTPS
#define HTTPS

#include <signal.h>
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

/**
 * @brief Create a http socket server
 * 
 * @param port port of socket
 * @return int server socket file descriptor
 */
int create_server(char* port);

/**
 * @brief Listens for http requests
 * 
 * @param sockfd server socket fd
 * @param queue socket queue
 * @param handle callback to handle an request
 * @param settings http server settings
 */
void server_listen(int sockfd, int queue, void (*handle)(struct req*, struct res*), struct settings* settings);

/**
 * @brief Suts the server down. On going connections will be fulfilled.
 * After the last connection closes the servers shuts down.
 * 
 */
void server_shutdown(void);

/**
 * @brief Sends a http response to client socket @code{clientfile}
 * 
 * @param clientfile Client socket file
 * @param res resposne struct
 */
void send_response(FILE* clientfile, struct res* res);

/**
 * @brief Parses the req line an sets method and path to the @code{req} struct.
 * The req line must be as followed: Method SP Status SP HTTP/1.1\r\n
 * 
 * @param line request line
 * @param req response struct
 * @return int 1 if req line is valid and could be parsed else -1
 */
int parse_req_line(char* line, struct req* req);

/**
 * @brief Gets the client socketfile and parses the requst.
 * 
 * @param client client socket file
 * @return struct req* request struct with parsed valus if the req is invalid NULL is returned
 * 
 */
struct req* parse_req(FILE* client);

#endif
