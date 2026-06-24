#include "recon/datastructures/SinglyLinkedList.h"

#include <iomanip>
#include <iostream>

namespace recon::datastructures {

SinglyLinkedList::~SinglyLinkedList() {
    Node* current = head_;
    while (current) {
        Node* next = current->next;
        delete current;
        current = next;
    }
}

SinglyLinkedList::SinglyLinkedList(SinglyLinkedList&& other) noexcept
    : head_(other.head_) {
    other.head_ = nullptr;  // leave the moved-from list empty, not dangling
}

SinglyLinkedList& SinglyLinkedList::operator=(SinglyLinkedList&& other) noexcept {
    if (this != &other) {
        Node* current = head_;
        while (current) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        head_ = other.head_;
        other.head_ = nullptr;
    }
    return *this;
}

void SinglyLinkedList::append(const PhoneRecord& record) {
    Node* newNode = new Node{record, nullptr};

    if (!head_) {
        head_ = newNode;
        return;
    }

    Node* current = head_;
    while (current->next) {
        current = current->next;
    }
    current->next = newNode;
}

void SinglyLinkedList::reverse() {
    Node* prev = nullptr;
    Node* current = head_;

    while (current) {
        Node* next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }

    head_ = prev;
}

void SinglyLinkedList::reversePairs() {
    if (!head_ || !head_->next) {
        return;
    }

    Node* prev = nullptr;
    Node* current = head_;
    head_ = current->next;

    while (current && current->next) {
        Node* nextPairStart = current->next->next;
        Node* pairHead = current->next;

        pairHead->next = current;
        current->next = nextPairStart;

        if (prev) {
            prev->next = pairHead;
        }

        prev = current;
        current = nextPairStart;
    }
}

void SinglyLinkedList::display() const {
    Node* current = head_;
    while (current) {
        std::cout << std::left << std::setw(20) << current->data.name
                  << current->data.number << "\n";
        current = current->next;
    }
}

std::size_t SinglyLinkedList::size() const {
    std::size_t count = 0;
    Node* current = head_;
    while (current) {
        count++;
        current = current->next;
    }
    return count;
}

bool SinglyLinkedList::empty() const {
    return head_ == nullptr;
}

}  // namespace recon::datastructures
