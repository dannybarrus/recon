#include "recon/datastructures/DoublyLinkedList.h"

#include <iomanip>
#include <iostream>

namespace recon::datastructures {

DoublyLinkedList::~DoublyLinkedList() {
    Node* current = head_;
    while (current) {
        Node* next = current->next;
        delete current;
        current = next;
    }
}

DoublyLinkedList::DoublyLinkedList(DoublyLinkedList&& other) noexcept
    : head_(other.head_) {
    other.head_ = nullptr;
}

DoublyLinkedList& DoublyLinkedList::operator=(DoublyLinkedList&& other) noexcept {
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

void DoublyLinkedList::insertSorted(const PhoneRecord& record) {
    Node* newNode = new Node{record, nullptr, nullptr};

    if (!head_ || record.name < head_->data.name) {
        newNode->next = head_;
        if (head_) {
            head_->prev = newNode;
        }
        head_ = newNode;
        return;
    }

    Node* current = head_;
    while (current->next && current->next->data.name < record.name) {
        current = current->next;
    }

    newNode->next = current->next;
    newNode->prev = current;
    if (current->next) {
        current->next->prev = newNode;
    }
    current->next = newNode;
}

void DoublyLinkedList::display() const {
    Node* current = head_;
    while (current) {
        std::cout << std::left << std::setw(20) << current->data.name
                  << current->data.number << "\n";
        current = current->next;
    }
}

std::size_t DoublyLinkedList::size() const {
    std::size_t count = 0;
    Node* current = head_;
    while (current) {
        count++;
        current = current->next;
    }
    return count;
}

bool DoublyLinkedList::empty() const {
    return head_ == nullptr;
}

}  // namespace recon::datastructures
