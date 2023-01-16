/**
 * @file httpc.c
 * @author Lorenz Hörburger 12024737
 * @brief Http client api
 *
 * @version 0.1
 * @date 15.01.2023
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef HTTPC
#define HTTPC

#include <stdio.h>

/**
 * @brief Sends a HTTP request with the given method
 * and writes the response to @code{output}
 * 
 * @param method request method
 * @param url url of http server
 * @param port port of server
 * @param output output file whre response should get written to
 * @return int 1 on success -1 on failure
 */
int httpc(const char* method, const char* url, const char* port, FILE* output);

/**
 * @brief Sends a http request to the socket @code{sockfile}
 * 
 * @param sockfile socket of http server
 * @param method request method
 * @param url url to access
 */

/**
 * @brief Creates a socket that connects to the http server
 * 
 * @param url req url
 * @param port port
 * @return FILE* opened socketfile
 */
FILE* create_socket(const char* url, const char* port);

/**
 * @brief Parses the response line and returns the response status.
 * Example HTTP/1.1 200 OK
 * 
 * @param line response line
 * @return int status or -1 if res line is invalid
 */
int parse_res_line(char* line);

#endif
