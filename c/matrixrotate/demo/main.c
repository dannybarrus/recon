#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "recon/matrix_rotate.h"

static void print_matrix(const int* matrix, int n, const char* label) {
    printf("  %s:\n", label);
    for (int row = 0; row < n; row++) {
        printf("    ");
        for (int col = 0; col < n; col++) {
            printf("%3d ", matrix[row * n + col]);
        }
        printf("\n");
    }
}

static int matrices_equal(const int* a, const int* b, int n) {
    return memcmp(a, b, sizeof(int) * (size_t)n * (size_t)n) == 0;
}

static void demo_sideways_camera_frame(void) {
    printf("--- a sideways-mounted camera frame, rotated in place ---\n");
    printf("  (a small 4x4 grayscale frame, stack-allocated -- no malloc anywhere)\n\n");

    int frame[4][4] = {
        {10, 20, 30, 40},
        {50, 60, 70, 80},
        {90, 100, 110, 120},
        {130, 140, 150, 160},
    };

    print_matrix((int*)frame, 4, "original frame (camera mounted sideways)");

    matrix_rotate_clockwise((int*)frame, 4);
    print_matrix((int*)frame, 4, "after rotating 90 degrees clockwise");

    printf("\n");
}

static void demo_round_trip_identity(void) {
    printf("--- round trip: clockwise then counterclockwise returns the original ---\n");

    int original[3][3] = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9},
    };
    int working[3][3];
    memcpy(working, original, sizeof(original));

    matrix_rotate_clockwise((int*)working, 3);
    matrix_rotate_counterclockwise((int*)working, 3);

    int matches = matrices_equal((int*)working, (int*)original, 3);
    printf("  rotate clockwise then counterclockwise == original: %s   (expected: yes)\n\n",
           matches ? "yes" : "NO -- MISMATCH");
}

static void demo_cross_check_against_naive_reference(void) {
    printf("--- cross-checking the in-place algorithm against a naive reference ---\n");
    printf("  Generating random matrices of many sizes, rotating each one both ways,\n");
    printf("  and confirming they produce IDENTICAL results -- this is how you\n");
    printf("  actually verify tricky index arithmetic, rather than trusting a hand\n");
    printf("  trace alone.\n\n");

    srand(42);
    int all_matched = 1;
    int trials = 0;

    for (int n = 1; n <= 30; n++) {
        for (int trial = 0; trial < 20; trial++) {
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

            if (!matrices_equal(in_place, naive, n)) {
                printf("  MISMATCH at n=%d, trial=%d\n", n, trial);
                all_matched = 0;
            }
            trials++;

            free(original);
            free(in_place);
            free(naive);
        }
    }

    printf("  %d trials across matrix sizes 1..30: %s\n\n", trials,
           all_matched ? "all matched the naive reference" : "SOME MISMATCHED -- see above");
}

static void demo_edge_cases(void) {
    printf("--- edge cases ---\n");

    int single[1][1] = {{42}};
    matrix_rotate_clockwise((int*)single, 1);
    printf("  1x1 matrix after rotation: %d   (expected: 42, unchanged -- a single\n", single[0][0]);
    printf("  element maps to itself under any rotation)\n");

    int pair[2][2] = {
        {1, 2},
        {3, 4},
    };
    matrix_rotate_clockwise((int*)pair, 2);
    printf("  2x2 [[1,2],[3,4]] rotated clockwise -> [[%d,%d],[%d,%d]]   (expected: [[3,1],[4,2]])\n\n",
           pair[0][0], pair[0][1], pair[1][0], pair[1][1]);
}

int main(void) {
    demo_sideways_camera_frame();
    demo_round_trip_identity();
    demo_edge_cases();
    demo_cross_check_against_naive_reference();
    return 0;
}
