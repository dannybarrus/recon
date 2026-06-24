#pragma once

#include <cstddef>

namespace recon::moderncpp {

// A deliberately expensive-to-copy resource -- owns a heap-allocated
// array of doubles. This is the instrumented vehicle for measuring move
// semantics' actual performance payoff, not just demonstrating that the
// code compiles.
//
// Copying a Buffer is O(size): a new array is allocated and every
// element is copied. Moving a Buffer is O(1): the pointer is simply
// transferred, and the source is left empty. For a large enough buffer,
// that difference is not subtle -- see the move semantics demo for an
// actual wall-clock comparison, mirroring the same "measure it, don't
// just claim it" rigor as CollectionPerformance in the Java module's
// collections package.
//
// copyCount() and moveCount() let tests assert exactly which
// constructor ran, rather than relying on inherently fuzzy wall-clock
// timing for correctness checks. The demo's timing numbers are for the
// human-readable "wow" moment; the counters are the actual proof.
class Buffer {
public:
    explicit Buffer(std::size_t size);
    ~Buffer();

    Buffer(const Buffer& other);
    Buffer& operator=(const Buffer& other);

    Buffer(Buffer&& other) noexcept;
    Buffer& operator=(Buffer&& other) noexcept;

    std::size_t size() const;
    double* data();
    const double* data() const;

    static int copyCount();
    static int moveCount();
    static void resetCounts();

private:
    std::size_t size_;
    double* data_;

    static int copyCount_;
    static int moveCount_;
};

}  // namespace recon::moderncpp
