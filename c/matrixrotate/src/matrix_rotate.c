#include "recon/matrix_rotate.h"

#define IDX(row, col, n) ((row) * (n) + (col))

void matrix_rotate_clockwise(int* matrix, int n) {
    for (int layer = 0; layer < n / 2; layer++) {
        int first = layer;
        int last = n - 1 - layer;

        for (int i = first; i < last; i++) {
            int offset = i - first;
            int top = matrix[IDX(first, i, n)];

            // left -> top
            matrix[IDX(first, i, n)] = matrix[IDX(last - offset, first, n)];
            // bottom -> left
            matrix[IDX(last - offset, first, n)] = matrix[IDX(last, last - offset, n)];
            // right -> bottom
            matrix[IDX(last, last - offset, n)] = matrix[IDX(i, last, n)];
            // top (saved) -> right
            matrix[IDX(i, last, n)] = top;
        }
    }
}

void matrix_rotate_counterclockwise(int* matrix, int n) {
    for (int layer = 0; layer < n / 2; layer++) {
        int first = layer;
        int last = n - 1 - layer;

        for (int i = first; i < last; i++) {
            int offset = i - first;
            int top = matrix[IDX(first, i, n)];

            // right -> top
            matrix[IDX(first, i, n)] = matrix[IDX(i, last, n)];
            // bottom -> right
            matrix[IDX(i, last, n)] = matrix[IDX(last, last - offset, n)];
            // left -> bottom
            matrix[IDX(last, last - offset, n)] = matrix[IDX(last - offset, first, n)];
            // top (saved) -> left
            matrix[IDX(last - offset, first, n)] = top;
        }
    }
}

void matrix_rotate_clockwise_naive_reference(const int* src, int* dst, int n) {
    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            dst[IDX(row, col, n)] = src[IDX(n - 1 - col, row, n)];
        }
    }
}
