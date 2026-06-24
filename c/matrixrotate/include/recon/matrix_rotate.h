#pragma once

// Rotates an N x N matrix 90 degrees, in place, using O(1) auxiliary
// space -- the classic constraint on this question: no second matrix,
// no malloc, nothing beyond a few scalar local variables.
//
// ─── Why this matters beyond being an interview question ──────────────────
//
// A real reason to actually need this: a camera mounted sideways on a
// piece of hardware (a 3D printer's bed-monitoring camera, a robot's
// sensor) captures frames in the wrong orientation, and the frame
// needs rotating before display or processing. On a memory-constrained
// embedded device, allocating a second full frame buffer just to
// rotate one is wasteful at best and impossible at worst -- the whole
// point of solving this in place is that it works directly on the
// buffer you already have, on the stack or in a fixed memory region,
// with no heap involved at all.
//
// ─── The technique: layer by layer, four-way cyclic swap ──────────────────
//
// Picture the matrix as a series of concentric square "rings" (layers),
// like an onion -- the outermost border is layer 0, the ring just
// inside it is layer 1, and so on toward the center. Within a single
// layer, each element on the TOP edge has a corresponding element on
// the RIGHT edge, the BOTTOM edge, and the LEFT edge that all need to
// cycle into each other's positions for a 90-degree rotation. Saving
// one of the four into a temporary, then moving each of the other
// three into the next one's old position, completes that four-way
// cycle using exactly one temporary variable -- not a second matrix.
//
// Walking every layer from the outside in, and every position along
// each layer's top edge, rotates the entire matrix with no auxiliary
// storage that scales with N at all.
//
// ─── Representation ─────────────────────────────────────────────────────────
//
// The matrix is a flattened 1D array (matrix[row * n + col]), not a
// genuine 2D array -- this is the realistic, most common way this
// question is actually answered, since C does not have first-class
// dynamically-sized 2D arrays without added complexity that isn't the
// actual point of this exercise. The rotation only ever moves whole
// int elements without inspecting their contents, so the same index
// arithmetic would work unchanged for any fixed-size element type.

void matrix_rotate_clockwise(int* matrix, int n);
void matrix_rotate_counterclockwise(int* matrix, int n);

// A deliberately naive reference implementation -- allocates a second
// matrix and copies directly via the rotation formula, with none of
// the in-place algorithm's cleverness. This exists ONLY so the tricky
// in-place version has something obviously-correct to be cross-checked
// against; it is not meant for any real use, and is exactly the kind
// of O(n^2)-extra-space approach the in-place version exists to avoid.
void matrix_rotate_clockwise_naive_reference(const int* src, int* dst, int n);
