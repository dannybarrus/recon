#include "unity.h"
#include "recon/gcode_tokenizer.h"

#include <string.h>

void setUp(void) {}
void tearDown(void) {}

void test_simple_word_tokens(void) {
    GCodeToken tokens[16];
    GCodeTokenizeResult result = gcode_tokenize_line("G1 X10", tokens, 16);

    TEST_ASSERT_TRUE(result.success);
    TEST_ASSERT_EQUAL_UINT(2, result.token_count);

    TEST_ASSERT_EQUAL_INT(GCODE_TOKEN_WORD, tokens[0].type);
    TEST_ASSERT_EQUAL_CHAR('G', tokens[0].letter);
    TEST_ASSERT_EQUAL_DOUBLE(1.0, gcode_word_value(&tokens[0]));

    TEST_ASSERT_EQUAL_INT(GCODE_TOKEN_WORD, tokens[1].type);
    TEST_ASSERT_EQUAL_CHAR('X', tokens[1].letter);
    TEST_ASSERT_EQUAL_DOUBLE(10.0, gcode_word_value(&tokens[1]));
}

void test_negative_decimal_value(void) {
    GCodeToken tokens[16];
    GCodeTokenizeResult result = gcode_tokenize_line("Y-20.3", tokens, 16);

    TEST_ASSERT_TRUE(result.success);
    TEST_ASSERT_EQUAL_UINT(1, result.token_count);
    TEST_ASSERT_EQUAL_DOUBLE(-20.3, gcode_word_value(&tokens[0]));
}

void test_token_points_into_original_buffer_not_a_copy(void) {
    const char* line = "G1 X10.5";
    GCodeToken tokens[16];
    GCodeTokenizeResult result = gcode_tokenize_line(line, tokens, 16);

    TEST_ASSERT_TRUE(result.success);
    TEST_ASSERT_EQUAL_PTR(line + 3, tokens[1].start);
}

void test_raw_text_span_matches_expected_substring(void) {
    const char* line = "G1 X10.5 Y20";
    GCodeToken tokens[16];
    GCodeTokenizeResult result = gcode_tokenize_line(line, tokens, 16);

    TEST_ASSERT_TRUE(result.success);
    TEST_ASSERT_EQUAL_UINT(5, tokens[1].length);  // "X10.5" is X,1,0,.,5 -- 5 characters
    TEST_ASSERT_EQUAL_INT(0, strncmp(tokens[1].start, "X10.5", 5));
}

void test_end_of_line_comment_captures_everything_after_semicolon(void) {
    GCodeToken tokens[16];
    GCodeTokenizeResult result = gcode_tokenize_line("G1 X10 ; move it Y20", tokens, 16);

    TEST_ASSERT_TRUE(result.success);
    TEST_ASSERT_EQUAL_UINT(3, result.token_count);  // G1, X10, and one comment token -- not 2, G1 counts too
    TEST_ASSERT_EQUAL_INT(GCODE_TOKEN_COMMENT, tokens[2].type);
    TEST_ASSERT_EQUAL_INT(0, strncmp(tokens[2].start, "; move it Y20", tokens[2].length));
}

void test_inline_comment_does_not_end_the_line(void) {
    GCodeToken tokens[16];
    GCodeTokenizeResult result = gcode_tokenize_line("G1 X10 (a comment) Y20", tokens, 16);

    TEST_ASSERT_TRUE(result.success);
    TEST_ASSERT_EQUAL_UINT(4, result.token_count);
    TEST_ASSERT_EQUAL_INT(GCODE_TOKEN_COMMENT, tokens[2].type);
    TEST_ASSERT_EQUAL_INT(GCODE_TOKEN_WORD, tokens[3].type);
    TEST_ASSERT_EQUAL_CHAR('Y', tokens[3].letter);
}

void test_unterminated_inline_comment_runs_to_end_of_line(void) {
    GCodeToken tokens[16];
    GCodeTokenizeResult result = gcode_tokenize_line("G1 (unterminated", tokens, 16);

    TEST_ASSERT_TRUE(result.success);
    TEST_ASSERT_EQUAL_UINT(2, result.token_count);
    TEST_ASSERT_EQUAL_INT(GCODE_TOKEN_COMMENT, tokens[1].type);
    TEST_ASSERT_EQUAL_INT(0, strncmp(tokens[1].start, "(unterminated", tokens[1].length));
}

void test_blank_line_produces_zero_tokens(void) {
    GCodeToken tokens[16];
    GCodeTokenizeResult result = gcode_tokenize_line("   ", tokens, 16);

    TEST_ASSERT_TRUE(result.success);
    TEST_ASSERT_EQUAL_UINT(0, result.token_count);
}

void test_comment_only_line(void) {
    GCodeToken tokens[16];
    GCodeTokenizeResult result = gcode_tokenize_line("; just a comment", tokens, 16);

    TEST_ASSERT_TRUE(result.success);
    TEST_ASSERT_EQUAL_UINT(1, result.token_count);
    TEST_ASSERT_EQUAL_INT(GCODE_TOKEN_COMMENT, tokens[0].type);
}

void test_letter_with_no_number_is_an_error(void) {
    GCodeToken tokens[16];
    GCodeTokenizeResult result = gcode_tokenize_line("G", tokens, 16);

    TEST_ASSERT_FALSE(result.success);
    TEST_ASSERT_EQUAL_UINT(0, result.error_position);
}

void test_letter_followed_by_space_then_digits_is_still_an_error(void) {
    GCodeToken tokens[16];
    GCodeTokenizeResult result = gcode_tokenize_line("G X10", tokens, 16);

    TEST_ASSERT_FALSE(result.success);
}

void test_unrecognized_character_is_an_error(void) {
    GCodeToken tokens[16];
    GCodeTokenizeResult result = gcode_tokenize_line("G1 @ X10", tokens, 16);

    TEST_ASSERT_FALSE(result.success);
    TEST_ASSERT_EQUAL_STRING("unrecognized character", result.error_message);
}

void test_too_many_tokens_for_buffer_is_reported_not_silently_truncated(void) {
    GCodeToken tokens[2];
    GCodeTokenizeResult result = gcode_tokenize_line("G1 X10 Y20", tokens, 2);

    TEST_ASSERT_FALSE(result.success);
    TEST_ASSERT_EQUAL_STRING("too many tokens for buffer", result.error_message);
}

void test_exact_fit_buffer_succeeds(void) {
    GCodeToken tokens[2];
    GCodeTokenizeResult result = gcode_tokenize_line("G1 X10", tokens, 2);

    TEST_ASSERT_TRUE(result.success);
    TEST_ASSERT_EQUAL_UINT(2, result.token_count);
}

void test_decimal_starting_with_dot_no_leading_digit(void) {
    GCodeToken tokens[16];
    GCodeTokenizeResult result = gcode_tokenize_line("X.5", tokens, 16);

    TEST_ASSERT_TRUE(result.success);
    TEST_ASSERT_EQUAL_DOUBLE(0.5, gcode_word_value(&tokens[0]));
}

void test_positive_sign_is_accepted(void) {
    GCodeToken tokens[16];
    GCodeTokenizeResult result = gcode_tokenize_line("X+5", tokens, 16);

    TEST_ASSERT_TRUE(result.success);
    TEST_ASSERT_EQUAL_DOUBLE(5.0, gcode_word_value(&tokens[0]));
}

typedef struct {
    const char* name;
    UnityTestFunction func;
} TestCase;

static const TestCase test_cases[] = {
    {"test_simple_word_tokens", test_simple_word_tokens},
    {"test_negative_decimal_value", test_negative_decimal_value},
    {"test_token_points_into_original_buffer_not_a_copy", test_token_points_into_original_buffer_not_a_copy},
    {"test_raw_text_span_matches_expected_substring", test_raw_text_span_matches_expected_substring},
    {"test_end_of_line_comment_captures_everything_after_semicolon",
     test_end_of_line_comment_captures_everything_after_semicolon},
    {"test_inline_comment_does_not_end_the_line", test_inline_comment_does_not_end_the_line},
    {"test_unterminated_inline_comment_runs_to_end_of_line", test_unterminated_inline_comment_runs_to_end_of_line},
    {"test_blank_line_produces_zero_tokens", test_blank_line_produces_zero_tokens},
    {"test_comment_only_line", test_comment_only_line},
    {"test_letter_with_no_number_is_an_error", test_letter_with_no_number_is_an_error},
    {"test_letter_followed_by_space_then_digits_is_still_an_error",
     test_letter_followed_by_space_then_digits_is_still_an_error},
    {"test_unrecognized_character_is_an_error", test_unrecognized_character_is_an_error},
    {"test_too_many_tokens_for_buffer_is_reported_not_silently_truncated",
     test_too_many_tokens_for_buffer_is_reported_not_silently_truncated},
    {"test_exact_fit_buffer_succeeds", test_exact_fit_buffer_succeeds},
    {"test_decimal_starting_with_dot_no_leading_digit", test_decimal_starting_with_dot_no_leading_digit},
    {"test_positive_sign_is_accepted", test_positive_sign_is_accepted},
};
#define TEST_CASE_COUNT (sizeof(test_cases) / sizeof(test_cases[0]))

int main(int argc, char** argv) {
    UNITY_BEGIN();

    if (argc > 1) {
        int found = 0;
        for (size_t i = 0; i < TEST_CASE_COUNT; i++) {
            if (strcmp(argv[1], test_cases[i].name) == 0) {
                UnityDefaultTestRun(test_cases[i].func, test_cases[i].name, __LINE__);
                found = 1;
                break;
            }
        }
        if (!found) {
            printf("Unknown test name: %s\n", argv[1]);
            return 1;
        }
    } else {
        for (size_t i = 0; i < TEST_CASE_COUNT; i++) {
            UnityDefaultTestRun(test_cases[i].func, test_cases[i].name, __LINE__);
        }
    }

    return UNITY_END();
}
