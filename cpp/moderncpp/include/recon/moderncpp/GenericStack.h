#pragma once

#include <stdexcept>
#include <vector>

namespace recon::moderncpp {

// A generic stack -- works for any type T, written once.
//
// This is the deferred payoff from datastructures/SinglyLinkedList and
// DoublyLinkedList, which were both deliberately written as concrete,
// non-generic classes specific to PhoneRecord. At the time, the
// reasoning was that genericity was being saved for this module --
// here it is. The same push/pop/top mechanics those classes implement
// by hand for PhoneRecord work here for any type at all, written
// exactly once.
//
// Built on std::vector<T> internally rather than a hand-rolled raw
// array -- the focus of THIS module is the template mechanism itself
// (how one definition serves arbitrary types), not re-deriving manual
// dynamic array management, which is already covered thoroughly
// elsewhere in this repo's C and C++ material.
template <typename T>
class GenericStack {
public:
    void push(const T& value) {
        data_.push_back(value);
    }

    void push(T&& value) {
        data_.push_back(std::move(value));
    }

    void pop() {
        if (empty()) {
            throw std::out_of_range("pop() called on an empty GenericStack");
        }
        data_.pop_back();
    }

    T& top() {
        if (empty()) {
            throw std::out_of_range("top() called on an empty GenericStack");
        }
        return data_.back();
    }

    const T& top() const {
        if (empty()) {
            throw std::out_of_range("top() called on an empty GenericStack");
        }
        return data_.back();
    }

    bool empty() const {
        return data_.empty();
    }

    std::size_t size() const {
        return data_.size();
    }

private:
    std::vector<T> data_;
};

}  // namespace recon::moderncpp
