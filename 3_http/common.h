#ifndef COMMON
#define COMMON
#include <stdio.h>

const char* prg_name;

int is_port_valid(const char* port);
char* file_from_url(const char* url);
int is_url_valid(const char* url);
char* file_path_from_url(const char* url);
void host_from_url(const char* url, char* host);
char* status_str(int status);
void get_rfc822_date(char* date);
int file_size(FILE* file);
char* resolve_path(char* docRoot, char* rel_url, char* index);

void log_error(const char* format, ...);

#endif
