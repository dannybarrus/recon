#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "recon/gcode_tokenizer.h"

static void print_token(const GCodeToken* token, const char* line) {
    if (token->type == GCODE_TOKEN_WORD) {
        printf("    WORD   letter='%c'  value=%g  raw=\"%.*s\"\n", token->letter,
               gcode_word_value(token), (int)token->length, token->start);
    } else {
        printf("    COMMENT raw=\"%.*s\"\n", (int)token->length, token->start);
    }
    ptrdiff_t offset = token->start - line;
    printf("            (points %td bytes into the original line buffer, not a copy)\n", offset);
}

static void demo_basic_tokenizing(void) {
    printf("--- tokenizing a real G-code move command ---\n");

    const char* line = "G1 X10.5 Y-20.3 Z0.2 F1500 ; first layer move";
    printf("  line: \"%s\"\n\n", line);

    GCodeToken tokens[16];
    GCodeTokenizeResult result = gcode_tokenize_line(line, tokens, 16);

    printf("  success: %s, token_count: %zu\n", result.success ? "yes" : "no", result.token_count);
    for (size_t i = 0; i < result.token_count; i++) {
        print_token(&tokens[i], line);
    }
    printf("\n");
}

static void demo_inline_vs_end_of_line_comments(void) {
    printf("--- two comment styles, two different behaviors ---\n");

    const char* inline_comment_line = "G1 X10 (move to position) Y20";
    printf("  line: \"%s\"\n", inline_comment_line);
    printf("  (parenthesized comment is INLINE -- parsing continues after it)\n");

    GCodeToken tokens[16];
    GCodeTokenizeResult result = gcode_tokenize_line(inline_comment_line, tokens, 16);
    printf("  token_count: %zu   (expected: 4 -- G1, X10, the comment, and Y20)\n", result.token_count);
    for (size_t i = 0; i < result.token_count; i++) {
        print_token(&tokens[i], inline_comment_line);
    }

    printf("\n");

    const char* eol_comment_line = "G1 X10 ; everything after this is ignored Y20";
    printf("  line: \"%s\"\n", eol_comment_line);
    printf("  (semicolon comment runs to end of line -- Y20 is NEVER parsed)\n");

    result = gcode_tokenize_line(eol_comment_line, tokens, 16);
    printf("  token_count: %zu   (expected: 3 -- G1, X10, and one comment token covering\n", result.token_count);
    printf("  everything after the semicolon, including the literal text \"Y20\")\n");
    for (size_t i = 0; i < result.token_count; i++) {
        print_token(&tokens[i], eol_comment_line);
    }
    printf("\n");
}

static void demo_error_handling(void) {
    printf("--- malformed input is reported, not silently misparsed ---\n");

    const char* bad_lines[] = {
        "G1 X10 @ Y20",
        "G1 X Y20",
        "G",
    };

    for (size_t i = 0; i < sizeof(bad_lines) / sizeof(bad_lines[0]); i++) {
        GCodeToken tokens[16];
        GCodeTokenizeResult result = gcode_tokenize_line(bad_lines[i], tokens, 16);
        printf("  \"%s\"\n", bad_lines[i]);
        printf("    success: %s", result.success ? "yes -- unexpected" : "no, as expected");
        if (!result.success) {
            printf("   error: \"%s\" at position %zu\n", result.error_message, result.error_position);
        } else {
            printf("\n");
        }
    }
    printf("\n");
}

static void demo_multi_line_program(void) {
    printf("--- tokenizing a small multi-line program ---\n");

    const char* program[] = {
        "; simple square outline",
        "G28 ; home all axes",
        "G1 X0 Y0 F1500",
        "G1 X50 Y0",
        "G1 X50 Y50",
        "G1 X0 Y50",
        "G1 X0 Y0",
    };

    for (size_t i = 0; i < sizeof(program) / sizeof(program[0]); i++) {
        GCodeToken tokens[16];
        GCodeTokenizeResult result = gcode_tokenize_line(program[i], tokens, 16);
        printf("  line %zu: \"%s\" -> %zu token(s)\n", i + 1, program[i], result.token_count);
    }
    printf("\n");
}

int main(void) {
    demo_basic_tokenizing();
    demo_inline_vs_end_of_line_comments();
    demo_error_handling();
    demo_multi_line_program();
    return 0;
}
