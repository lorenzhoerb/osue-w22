/**
 * @file hexlib.c
 * @author Lorenz Hörburger 12024737
 * @brief Hex libarary for working with hex strings.
 *
 * @version 0.1
 * @date 2022-09-12
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "hexlib.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int is_hex_str(const char* str)
{
    if (str[strspn(str, "0123456789abcdefABCDEF")] == 0) {
        return 1;
    }
    return 0;
}

int mult_hex_chars(char a, char b)
{
    int ia = hex_char_to_int(a);
    int ib = hex_char_to_int(b);
    return ia * ib;
}

int set_leading_zeros(char** a)
{
    size_t len = strlen(*a);
    double power = ceil(log2(len));
    size_t eq_len = pow(2, power);
    size_t leading = eq_len - len;

    if (fill_zero(leading, a) == -1) {
        return -1;
    }
    return 0;
}

int equalize_zeros(char** a, char** b)
{
    size_t len_a = strlen(*a);
    size_t len_b = strlen(*b);
    size_t maxlen = len_a > len_b ? len_a : len_b;
    double power = ceil(log2(maxlen));
    size_t eq_len = pow(2, power);
    size_t leading_a = eq_len - len_a;
    size_t leading_b = eq_len - len_b;

    if (fill_zero(leading_a, a) == -1 || fill_zero(leading_b, b) == -1) {
        return -1;
    }
    return 0;
}

int fill_zero(uint8_t zeros, char** str)
{
    char tmp_str[strlen(*str) + 1];
    strcpy(tmp_str, *str);
    int new_len = zeros + strlen(*str) + 1;
    *str = (char*)realloc(*str, new_len);
    if (str == NULL) {
        return -1;
    }
    memset(*str, '\0', new_len);
    memset(*str, '0', zeros);
    strcat(*str, tmp_str);
    return 0;
}

int hex_char_to_int(char character)
{
    if (character >= '0' && character <= '9')
        return character - '0';
    if (character >= 'A' && character <= 'F')
        return character - 'A' + 10;
    if (character >= 'a' && character <= 'f')
        return character - 'a' + 10;
    return -1;
}

char int_to_hex_char(int i)
{
    if (i < 10)
        return '0' + i;
    else
        return 'a' + i - 10;
}

char* add_hex_str(char* a, char* b, int offset)
{
    int a_pos = strlen(a) - 1;
    int b_pos = strlen(b) - 1;
    char out_hex[a_pos + b_pos + 2 + 2];

    int x_hex = 0, y_hex = 0, result = 0, overflow = 0, w_pos = 0, tmp_result = 0;

    while (a_pos >= 0 || b_pos >= 0) {
        tmp_result = 0;
        if (a_pos >= 0) {
            x_hex = hex_char_to_int(a[a_pos]);
            tmp_result += x_hex;
            a_pos--;
        }

        if (b_pos >= 0 && offset <= 0) {
            y_hex = hex_char_to_int(b[b_pos]);
            tmp_result += y_hex;
            b_pos--;
        }
        tmp_result += overflow;
        overflow = tmp_result / 16;
        result = tmp_result % 16;
        out_hex[w_pos++] = int_to_hex_char(result);
        offset--;
    }

    if (overflow != 0) {
        out_hex[w_pos++] = int_to_hex_char(overflow);
    }

    out_hex[w_pos] = '\0';

    char* endResult = malloc(strlen(out_hex) + 2);
    memset(endResult, '\0', strlen(out_hex) + 2);
    if (endResult == NULL) {
        return NULL;
    }

    // reverse out_hex and put it into a
    int i;
    for (i = strlen(out_hex) - 1; i >= 0; i--) {
        endResult[strlen(out_hex) - 1 - i] = out_hex[i];
    }
    return endResult;
}
