/**
 * @file common.h
 * @author Lorenz Hörburger 12024737
 * @brief Common mothods for http client and http server
 *
 * @version 0.1
 * @date 15.01.2023
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef COMMON
#define COMMON
#include <stdio.h>

const char* prg_name;

/**
 * @brief Validates if the given port is valid.
 * A port is valid if its 0 >= port <= 65535
 * 
 * @param port port as 0 terminated string
 * @return int returns 1 if valid 0 if invalid
 */
int is_port_valid(const char* port);

/**
 * @brief Gets the file form an url if a file is specified.
 * 
 * Examples: 
 * http://localhost/test/abc.html would return abc.html
 * http://localhost/ would return null because no file is specified
 * 
 * @param url valid url eg http://localhost/this/index.html
 * @return char* file if present else null
 */
char* file_from_url(const char* url);

/**
 * @brief Checks if the given url is valid.
 * A URL is valid if it starts with http:// and after the hostname / follows.
 * 
 * @param url url
 * @return int 1 if valid else 0
 */
int is_url_valid(const char* url);

/**
 * @brief Gets the whole file path from an url.
 *  Example: http://localhost/abc/def/index.html would return 
 * /abc/def/index.html
 * 
 * @param url valid url
 * @return char* file path
 */
char* file_path_from_url(const char* url);

/**
 * @brief Gets the host from a valid url.
 * Example http://test.abc.at/ would return test.abc.at
 * 
 * @param url valid url
 * @param host host
 */
void host_from_url(const char* url, char* host);

/**
 * @brief Returns the description of a REST status code.
 * 
 * @param status REST status code
 * @return char* description of status code if defined else NULL
 */
char* status_str(int status);

/**
 * @brief Get the rfc822 date and saves it into date
 * 
 * @param date buffer to save date into
 */
void get_rfc822_date(char* date);

/**
 * @brief Gets the size in bytes of a file.
 * 
 * @param file opened file
 * @return int size in byte
 */
int file_size(FILE* file);

/**
 * @brief Resolves the path for the given parameters.
 * if rel_url accesses a specific file the the resolved path
 * is docRoot + rel_url. If no file is specified the index 
 * will be appended.
 * This function allocates memory. Memorey must be freed.
 * 
 * @param docRoot document root
 * @param rel_url relative url
 * @param index default file
 * @return char* resolved path
 */
char* resolve_path(char* docRoot, char* rel_url, char* index);

/**
 * @brief Logs and error to stdrr. Supports all the 
 * printf formats.
 * 
 * @param format formats
 * @param ... 
 */
void log_error(const char* format, ...);

#endif
