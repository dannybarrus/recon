#include "recon/gcode_tokenizer.h"

#include <ctype.h>
#include <stdlib.h>

// Scans a number starting at `p`: optional sign, digits, optional '.'
// followed by more digits. Returns a pointer one past the last
// character consumed, or NULL if no digits were actually found at all
// (a bare sign or dot with nothing else doesn't count as a number).
//
// Deliberately does NOT recognize scientific notation (e.g. "1e5") --
// real G-code, whether hand-written or slicer-generated, uses plain
// decimal values for coordinates and parameters; scientific notation
// essentially never appears in practice. Supporting it would also
// require this scanner and gcode_word_value()'s strtod() call to agree
// exactly on where a number ends, which matters because the TOKEN
// BOUNDARY itself is determined here, before strtod ever runs.
static const char* scan_number(const char* p) {
    if (*p == '-' || *p == '+') {
        p++;
    }
    int saw_digit = 0;
    while (isdigit((unsigned char)*p)) {
        p++;
        saw_digit = 1;
    }
    if (*p == '.') {
        p++;
        while (isdigit((unsigned char)*p)) {
            p++;
            saw_digit = 1;
        }
    }
    return saw_digit ? p : NULL;
}

GCodeTokenizeResult gcode_tokenize_line(const char* line, GCodeToken* tokens, size_t max_tokens) {
    GCodeTokenizeResult result = {.success = 1, .token_count = 0, .error_message = NULL, .error_position = 0};
    const char* p = line;

    while (*p != '\0') {
        if (isspace((unsigned char)*p)) {
            p++;
            continue;
        }

        if (*p == ';') {
            if (result.token_count >= max_tokens) {
                result.success = 0;
                result.error_message = "too many tokens for buffer";
                result.error_position = (size_t)(p - line);
                return result;
            }
            size_t remaining_len = 0;
            while (p[remaining_len] != '\0') {
                remaining_len++;
            }
            tokens[result.token_count].type = GCODE_TOKEN_COMMENT;
            tokens[result.token_count].start = p;
            tokens[result.token_count].length = remaining_len;
            tokens[result.token_count].letter = '\0';
            result.token_count++;
            p += remaining_len;
            break;
        }

        if (*p == '(') {
            const char* start = p;
            p++;
            while (*p != '\0' && *p != ')') {
                p++;
            }
            if (*p == ')') {
                p++;
            }

            if (result.token_count >= max_tokens) {
                result.success = 0;
                result.error_message = "too many tokens for buffer";
                result.error_position = (size_t)(start - line);
                return result;
            }
            tokens[result.token_count].type = GCODE_TOKEN_COMMENT;
            tokens[result.token_count].start = start;
            tokens[result.token_count].length = (size_t)(p - start);
            tokens[result.token_count].letter = '\0';
            result.token_count++;
            continue;
        }

        if (isalpha((unsigned char)*p)) {
            const char* start = p;
            char letter = *p;
            p++;

            const char* number_end = scan_number(p);
            if (number_end == NULL) {
                result.success = 0;
                result.error_message = "expected a number after command letter";
                result.error_position = (size_t)(start - line);
                return result;
            }

            if (result.token_count >= max_tokens) {
                result.success = 0;
                result.error_message = "too many tokens for buffer";
                result.error_position = (size_t)(start - line);
                return result;
            }

            tokens[result.token_count].type = GCODE_TOKEN_WORD;
            tokens[result.token_count].start = start;
            tokens[result.token_count].length = (size_t)(number_end - start);
            tokens[result.token_count].letter = letter;
            result.token_count++;
            p = number_end;
            continue;
        }

        result.success = 0;
        result.error_message = "unrecognized character";
        result.error_position = (size_t)(p - line);
        return result;
    }

    return result;
}

double gcode_word_value(const GCodeToken* word_token) {
    return strtod(word_token->start + 1, NULL);
}
