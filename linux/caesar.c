#include "caesar.h"
#include <ctype.h>
#include <string.h>

// Función para rotar letras (mayúsculas y minúsculas)
static char rot_alpha(char c, int shift) {
    if (islower(c)) {
        return (char)((((c - 'a') + shift) % 26) + 'a');
    }
    if (isupper(c)) {
        return (char)((((c - 'A') + shift) % 26) + 'A');
    }
    return c;
}

// Función para rotar dígitos
static char rot_digit(char c, int shift) {
    if (isdigit(c)) {
        return (char)((((c - '0') + (shift % 10)) % 10) + '0');
    }
    return c;
}

size_t caesar_encrypt_bytes(const char* plaintext, uint8_t shift, uint8_t* out, size_t out_cap) {
    if (!out || out_cap == 0) return 0;

    // El primer byte es el shift
    out[0] = shift % 26;
    size_t written_bytes = 1;

    for (size_t i = 0; plaintext[i] != '\0'; ++i) {
        if (written_bytes >= out_cap) break;
        char ch = plaintext[i];
        if (isalpha(ch)) {
            out[written_bytes++] = (uint8_t)rot_alpha(ch, shift);
        } else if (isdigit(ch)) {
            out[written_bytes++] = (uint8_t)rot_digit(ch, shift);
        } else {
            out[written_bytes++] = (uint8_t)ch;
        }
    }
    return written_bytes;
}

size_t caesar_decrypt_bytes(const uint8_t* in, size_t in_len, char* out, size_t out_cap) {
    if (!in || in_len < 1 || !out || out_cap == 0) return 0;

    uint8_t shift = in[0] % 26;
    int inv_alpha_shift = 26 - shift;
    int inv_digit_shift = 10 - (shift % 10);
    size_t written_bytes = 0;

    for (size_t i = 1; i < in_len; ++i) {
        if (written_bytes + 1 >= out_cap) break;
        char ch = (char)in[i];
        if (isalpha(ch)) {
            out[written_bytes++] = rot_alpha(ch, inv_alpha_shift);
        } else if (isdigit(ch)) {
            out[written_bytes++] = rot_digit(ch, inv_digit_shift);
        } else {
            out[written_bytes++] = ch;
        }
    }
    out[written_bytes] = '\0'; // NUL-terminator
    return written_bytes;
}