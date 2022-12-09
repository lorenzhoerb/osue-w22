#ifndef HEXLIB
#define HEXLIB

#include <stdint.h>

/**
 * @brief Validates if the null-terminated string is valid hex.
 * Following chars are accepted: [0-9a-fA-F]
 *
 * @param str Null-terminated string
 * @return int 1 if the given string is valid hex else 0
 */
int is_hex_str(const char* str);

/**
 * @brief Fills the string with @code{zeros} leading zeros.
 *
 * @param zeros Zeros to fill
 * @param str Allocated char buffer to be filled with leading zeros
 */
int fill_zero(uint8_t zeros, char** str);

int equalize_zeros(char** a, char** b);

/**
 * @brief Parses a hex-char to a int
 * valid input chars [0-9a-zA-Z]
 *
 * @param character hex-char
 * @return int, hex-char as int. Returns -1 if the the char couldn't be parsed
 */
int hex_char_to_int(char character);

int mult_hex_chars(char a, char b);

int set_leading_zeros(char** a);

/**
 * @brief parses a int to a hex-char
 *
 *
 * @param i input int to be parsed to a hex char
 * @return char, parsed char.
 */
char int_to_hex_char(int i);

/**
 * @brief
 *
 * @param a hex character
 * @param b hex character
 * @return char
 */
char add_hex(char a, char b, char* overflow);

char* add_hex_str(char* a, char* b, int offset);

#endif
