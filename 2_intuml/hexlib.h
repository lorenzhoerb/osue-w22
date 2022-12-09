/**
 * @file hexlib.h
 * @author Lorenz Hörburger 12024737
 * @brief Hex libarary for working with hex strings.
 *
 * @version 0.1
 * @date 2022-09-12
 *
 * @copyright Copyright (c) 2022
 *
 */
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

/**
 * @brief Equalizes two strings so that they have the same length.
 * The length must be a power of 2. If these conditions are not fullfilled
 * the strings a filled with leading zeros.
 *
 * @param a allocated string a
 * @param b allocated string b
 * @return int 0 on success -1 when an error occurred
 */
int equalize_zeros(char** a, char** b);

/**
 * @brief Parses a hex-char to a int
 * valid input chars [0-9a-zA-Z]
 *
 * @param character hex-char
 * @return int, hex-char as int. Returns -1 if the the char couldn't be parsed
 */
int hex_char_to_int(char character);

/**
 * @brief Set leading zeros so that the length of a is a power of 2.
 *
 * @param a allocated string a
 * @return int 0 on success -1 on failure
 */
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
 * @brief Multiplies two hex chars.
 *
 * @param a hex char a
 * @param b hex char b
 * @return int product of multiplication in dezimal
 */
int mult_hex_chars(char a, char b);

/**
 * @brief Addition on two hex chars and returns the result.
 * The result gets allocated and must be freed.
 *
 * @param a hex string a
 * @param b hex string b
 * @param offset Offsets b to the left by offset digits
 * @return char* allocated result
 */
char* add_hex_str(char* a, char* b, int offset);

#endif
