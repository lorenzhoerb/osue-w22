#include "hexlib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int is_hex_str(const char *str) {
    if (str[strspn(str, "0123456789abcdefABCDEF")] == 0) {
        return 1;
    }
    return 0;
}

int mult_hex_chars(char a, char b) {
    int ia = hex_char_to_int(a);
    int ib = hex_char_to_int(b);
    return ia * ib;
}

int equalize_zeros(char **a, char **b) {
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

int fill_zero(uint8_t zeros, char **str) {
    char tmp_str[strlen(*str) + 1];
    strcpy(tmp_str, *str);
    int new_len = zeros + strlen(*str) + 1;
    *str = (char *)realloc(*str, new_len);
    if (str == NULL) {
        return -1;
    }
    memset(*str, '\0', new_len);
    memset(*str, '0', zeros);
    strcat(*str, tmp_str);
    return 0;
}

int hex_char_to_int(char character) {
    if (character >= '0' && character <= '9')
        return character - '0';
    if (character >= 'A' && character <= 'F')
        return character - 'A' + 10;
    if (character >= 'a' && character <= 'f')
        return character - 'a' + 10;
    return -1;
}
