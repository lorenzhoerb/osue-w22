/**
 * @file log.c
 * @author Lorenz Hörburger (12024737)
 * @brief Implementation of the log.h
 * @version 0.1
 * @date 2022-11-10
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "log.h"
#include <stdarg.h>
#include <stdio.h>

void log_error(const char* format, ...)
{
    va_list args;
    fprintf(stderr, "%s ERROR: ", prg_name);
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
}
