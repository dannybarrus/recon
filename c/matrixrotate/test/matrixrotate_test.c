#include "unity.h"
#include "recon/matrix_rotate.h"

#include <stdlib.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

static int matrices_equal(const int* a, const int* b, int n) {
    return memcmp(a, b, sizeof(int) * (size_t)n * (size_t)n) == 0;
}

void test_1x1_matrix_is_unchanged(void) {
    int m[1] = {42};
    matrix_rotate_clockwise(m, 1);
    TEST_ASSERT_EQUAL(42, m[0]);
}

void test_0x0_matrix_does_not_crash(void) {
    int m[1] = {0};
    matrix_rotate_clockwise(m, 0);
    TEST_PASS();
}

void test_2x2_clockwise_known_values(void) {
    int m[4] = {
        1, 2,
        3, 4,
    };
    int expected[4] = {
        3, 1,
        4, 2,
    };
    matrix_rotate_clockwise(m, 2);
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, m, 4);
}

void test_3x3_clockwise_known_values(void) {
    int m[9] = {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9,
    };
    int expected[9] = {
        7, 4, 1,
        8, 5, 2,
        9, 6, 3,
    };
    matrix_rotate_clockwise(m, 3);
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, m, 9);
}

void test_4x4_clockwise_known_values(void) {
    int m[16] = {
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 16,
    };
    int expected[16] = {
        13, 9, 5, 1,
        14, 10, 6, 2,
        15, 11, 7, 3,
        16, 12, 8, 4,
    };
    matrix_rotate_clockwise(m, 4);
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, m, 16);
}

void test_counterclockwise_is_the_inverse_of_clockwise(void) {
    int original[9] = {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9,
    };
    int working[9];
    memcpy(working, original, sizeof(original));

    matrix_rotate_clockwise(working, 3);
    matrix_rotate_counterclockwise(working, 3);

    TEST_ASSERT_EQUAL_INT_ARRAY(original, working, 9);
}

void test_four_clockwise_rotations_return_to_original(void) {
    int original[16] = {
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 16,
    };
    int working[16];
    memcpy(working, original, sizeof(original));

    for (int i = 0; i < 4; i++) {
        matrix_rotate_clockwise(working, 4);
    }

    TEST_ASSERT_EQUAL_INT_ARRAY(original, working, 16);
}

void test_naive_reference_matches_known_4x4_result(void) {
    int src[16] = {
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 16,
    };
    int dst[16];
    int expected[16] = {
        13, 9, 5, 1,
        14, 10, 6, 2,
        15, 11, 7, 3,
        16, 12, 8, 4,
    };

    matrix_rotate_clockwise_naive_reference(src, dst, 4);
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, dst, 16);
}

void test_in_place_matches_naive_reference_across_many_random_matrices(void) {
    srand(1234);

    for (int n = 1; n <= 25; n++) {
        for (int trial = 0; trial < 10; trial++) {
            int* original = malloc(sizeof(int) * (size_t)n * (size_t)n);
            int* in_place = malloc(sizeof(int) * (size_t)n * (size_t)n);
            int* naive = malloc(sizeof(int) * (size_t)n * (size_t)n);

            for (int i = 0; i < n * n; i++) {
                int value = rand() % 1000;
                original[i] = value;
                in_place[i] = value;
            }

            matrix_rotate_clockwise(in_place, n);
            matrix_rotate_clockwise_naive_reference(original, naive, n);

            TEST_ASSERT_TRUE_MESSAGE(matrices_equal(in_place, naive, n), "in-place result diverged from naive reference");

            free(original);
            free(in_place);
            free(naive);
        }
    }
}

typedef struct {
    const char* name;
    UnityTestFunction func;
} TestCase;

static const TestCase test_cases[] = {
    {"test_1x1_matrix_is_unchanged", test_1x1_matrix_is_unchanged},
    {"test_0x0_matrix_does_not_crash", test_0x0_matrix_does_not_crash},
    {"test_2x2_clockwise_known_values", test_2x2_clockwise_known_values},
    {"test_3x3_clockwise_known_values", test_3x3_clockwise_known_values},
    {"test_4x4_clockwise_known_values", test_4x4_clockwise_known_values},
    {"test_counterclockwise_is_the_inverse_of_clockwise", test_counterclockwise_is_the_inverse_of_clockwise},
    {"test_four_clockwise_rotations_return_to_original", test_four_clockwise_rotations_return_to_original},
    {"test_naive_reference_matches_known_4x4_result", test_naive_reference_matches_known_4x4_result},
    {"test_in_place_matches_naive_reference_across_many_random_matrices",
     test_in_place_matches_naive_reference_across_many_random_matrices},
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
