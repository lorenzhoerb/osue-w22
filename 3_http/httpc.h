#ifndef HTTPC
#define HTTPC

#include <stdio.h>

int httpc(const char* method, const char* url, const char* port, FILE* output);

#endif
