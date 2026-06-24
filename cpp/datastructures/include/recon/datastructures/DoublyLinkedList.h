#pragma once

#include <cstddef>

#include "recon/datastructures/PhoneRecord.h"

namespace recon::datastructures {

// A doubly linked list that keeps itself sorted by name as records are
// inserted -- one insertSorted() call per record.
//
// BUG FOUND AND FIXED: the original's CreateList() built the entire
// sorted list in one function, with a single `pPrev` variable declared
// ONCE outside the outer loop and never reset between processing
// different records. Each new record's insertion search reused
// whatever pPrev was left over from the PREVIOUS record's insertion,
// rather than starting fresh from "nothing before the head." That's a
// stale-pointer bug that depends on the exact sequence of insertions to
// manifest -- exactly the kind of bug that's easy to miss by inspection
// and easy to get unlucky with in production.
//
// The fix here isn't a patch to that loop -- it's a structural change.
// insertSorted() is a single, fully self-contained call: it always
// starts its search from head_ and never depends on any state left
// over from a previous call. There is no shared mutable variable for a
// later call to accidentally inherit a stale value from.
//
// Also replaces the original's manual DeleteList() free-function
// pattern with a real destructor (RAII) -- forgetting to call
// DeleteList(), or an exception thrown before it ran, would leak every
// node forever in the original. A destructor runs automatically and
// unconditionally.
//
// Copy construction and copy assignment are explicitly disabled (the
// minimal form of the "Rule of Three"): this class owns raw pointers
// directly, and the compiler-generated default copy would just copy the
// head_ pointer value itself -- two DoublyLinkedList objects would then
// both believe they own the same nodes, and both destructors would
// delete the same memory. Disabling copy makes that bug impossible to
// write instead of just unlikely.
//
// Move construction and move assignment ARE provided (completing the
// "Rule of Five") -- see SinglyLinkedList's header for why disabling
// copy without also providing move is a real trap, not just a style
// choice: it makes a type impossible to return by value.
class DoublyLinkedList {
public:
    DoublyLinkedList() = default;
    ~DoublyLinkedList();

    DoublyLinkedList(const DoublyLinkedList&) = delete;
    DoublyLinkedList& operator=(const DoublyLinkedList&) = delete;

    DoublyLinkedList(DoublyLinkedList&& other) noexcept;
    DoublyLinkedList& operator=(DoublyLinkedList&& other) noexcept;

    // Inserts a record, keeping the list sorted by name (ascending).
    void insertSorted(const PhoneRecord& record);

    // Prints every record, one per line, head to tail.
    void display() const;

    std::size_t size() const;
    bool empty() const;

private:
    struct Node {
        PhoneRecord data;
        Node* next = nullptr;
        Node* prev = nullptr;
    };

    Node* head_ = nullptr;
};

}  // namespace recon::datastructures
