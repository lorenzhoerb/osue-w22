#ifndef COMMON
#define COMMON

const char* prg_name;

int is_port_valid(const char* port);
char* file_from_url(const char* url);
int is_url_valid(const char* url);
char* file_path_from_url(const char* url);
void host_from_url(const char* url, char* host);

void log_error(const char* format, ...);

#endif
