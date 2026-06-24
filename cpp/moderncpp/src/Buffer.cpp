#include "recon/moderncpp/Buffer.h"

#include <algorithm>
#include <utility>

namespace recon::moderncpp {

int Buffer::copyCount_ = 0;
int Buffer::moveCount_ = 0;

Buffer::Buffer(std::size_t size) : size_(size), data_(new double[size]) {
    for (std::size_t i = 0; i < size_; i++) {
        data_[i] = static_cast<double>(i);
    }
}

Buffer::~Buffer() {
    delete[] data_;
}

Buffer::Buffer(const Buffer& other) : size_(other.size_), data_(new double[other.size_]) {
    std::copy(other.data_, other.data_ + other.size_, data_);
    ++copyCount_;
}

Buffer& Buffer::operator=(const Buffer& other) {
    if (this != &other) {
        double* newData = new double[other.size_];
        std::copy(other.data_, other.data_ + other.size_, newData);
        delete[] data_;
        data_ = newData;
        size_ = other.size_;
        ++copyCount_;
    }
    return *this;
}

Buffer::Buffer(Buffer&& other) noexcept
    : size_(other.size_), data_(other.data_) {
    other.data_ = nullptr;
    other.size_ = 0;
    ++moveCount_;
}

Buffer& Buffer::operator=(Buffer&& other) noexcept {
    if (this != &other) {
        delete[] data_;
        data_ = other.data_;
        size_ = other.size_;
        other.data_ = nullptr;
        other.size_ = 0;
        ++moveCount_;
    }
    return *this;
}

std::size_t Buffer::size() const {
    return size_;
}

double* Buffer::data() {
    return data_;
}

const double* Buffer::data() const {
    return data_;
}

int Buffer::copyCount() {
    return copyCount_;
}

int Buffer::moveCount() {
    return moveCount_;
}

void Buffer::resetCounts() {
    copyCount_ = 0;
    moveCount_ = 0;
}

}  // namespace recon::moderncpp
