/**
 * @file log.h
 * @author Lorenz Hörburger (12024737)
 * @brief Definitions for the log object
 * @version 0.1
 * @date 2022-11-10
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef LOG
#define LOG

// global program name
const char* prg_name;

/**
 * @brief Logs the error message specified in format to stderr.
 * prg_name needs to be set before calling this methode
 *
 * @param format Error message as formatted char array
 * @param ... format parameters
 */
void log_error(const char* format, ...);

#endif
