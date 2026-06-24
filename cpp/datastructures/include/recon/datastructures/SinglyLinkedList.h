#pragma once

#include <cstddef>

#include "recon/datastructures/PhoneRecord.h"

namespace recon::datastructures {

// Consolidates two original files -- lnklstreverse.cpp (reverse the
// whole list) and lnklstrevpairs.cpp (reverse adjacent pairs) -- into
// one class with two operations, the same way this repo's Java module
// consolidated two near-identical NetBeans LinkedList implementations
// into one generic LinkedList<T>.
//
// reverse() mirrors the original lnklstreverse.cpp algorithm, which was
// already correct: the standard three-pointer iterative reversal.
//
// reversePairs() mirrors lnklstrevpairs.cpp, which was ALSO already
// correct -- verified by hand-tracing both an even-length and an
// odd-length list through the algorithm during the rewrite, then
// confirmed again here by an actual compiled test rather than trusting
// the hand trace alone. lnklstrevpairs.cpp was, by a comfortable margin,
// the best-engineered file in the original cpp/ folder: correct logic,
// real documentation explaining the pointer-to-pointer design decision,
// and non-interactive sample data generation -- the pattern every other
// file in this consolidated class now follows too.
//
// Copy construction and copy assignment are explicitly disabled (the
// minimal form of the "Rule of Three"): this class owns raw pointers
// directly, and the compiler-generated default copy would just copy the
// head_ pointer value itself -- two SinglyLinkedList objects would then
// both believe they own the same nodes, and both destructors would
// delete the same memory. Disabling copy makes that bug impossible to
// write instead of just unlikely.
//
// Move construction and move assignment ARE provided -- this is the
// "Rule of Five" completing the "Rule of Three." Disabling copy without
// providing move is a real trap: in C++11 and later, declaring a copy
// constructor (even as `= delete`) suppresses the compiler's implicitly
// generated move constructor entirely. Without an explicit move, this
// class could not be returned by value from a function, could not be
// stored in a std::vector that ever needs to reallocate, and could not
// be used in most idiomatic "build it and hand it back" patterns -- this
// was discovered directly while writing this class's own test helpers,
// which build a list in a function and return it.
class SinglyLinkedList {
public:
    SinglyLinkedList() = default;
    ~SinglyLinkedList();

    SinglyLinkedList(const SinglyLinkedList&) = delete;
    SinglyLinkedList& operator=(const SinglyLinkedList&) = delete;

    SinglyLinkedList(SinglyLinkedList&& other) noexcept;
    SinglyLinkedList& operator=(SinglyLinkedList&& other) noexcept;

    // Appends a record to the end of the list.
    void append(const PhoneRecord& record);

    // Reverses the entire list in place. [1,2,3,4] -> [4,3,2,1]
    void reverse();

    // Reverses each adjacent pair in place, leaving a trailing odd
    // element (if any) in its original position.
    // [1,2,3,4]   -> [2,1,4,3]
    // [1,2,3,4,5] -> [2,1,4,3,5]
    void reversePairs();

    void display() const;

    std::size_t size() const;
    bool empty() const;

private:
    struct Node {
        PhoneRecord data;
        Node* next = nullptr;
    };

    Node* head_ = nullptr;
};

}  // namespace recon::datastructures
