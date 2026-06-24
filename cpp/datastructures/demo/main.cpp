#include <iostream>
#include <sstream>

#include "recon/datastructures/DoublyLinkedList.h"
#include "recon/datastructures/SinglyLinkedList.h"

using namespace recon::datastructures;

namespace {

// Generates sample records programmatically rather than blocking on
// console input -- the original doublylinkedlist.cpp and
// lnklstreverse.cpp both called gets_s() in a loop, requiring a human
// to sit and type names and numbers before either program would do
// anything. That makes a program impossible to run unattended (no CI
// smoke test, no quick demo without manual typing). lnklstrevpairs.cpp
// already avoided this with its own AutoCreateList() -- extended here
// to every file in this folder.
PhoneRecord makeRecord(int n) {
    std::ostringstream name;
    std::ostringstream number;
    name << "Contact " << n;
    number << "555-01" << (10 + n);
    return PhoneRecord{name.str(), number.str()};
}

void demoDoublyLinkedList() {
    std::cout << "--- DoublyLinkedList: sorted insertion ---\n";

    DoublyLinkedList list;
    list.insertSorted({"Charlie", "555-0103"});
    list.insertSorted({"Alice", "555-0101"});
    list.insertSorted({"Echo", "555-0105"});
    list.insertSorted({"Bob", "555-0102"});
    list.insertSorted({"Delta", "555-0104"});

    std::cout << "Inserted in order: Charlie, Alice, Echo, Bob, Delta\n";
    std::cout << "Display (expected alphabetical order):\n";
    list.display();
    std::cout << "size() = " << list.size() << "\n\n";
}

void demoSinglyLinkedListReverse() {
    std::cout << "--- SinglyLinkedList: reverse() ---\n";

    SinglyLinkedList list;
    for (int i = 1; i <= 5; i++) {
        list.append(makeRecord(i));
    }

    std::cout << "Before reverse:\n";
    list.display();

    list.reverse();

    std::cout << "After reverse (expected: Contact 5..Contact 1):\n";
    list.display();
    std::cout << "\n";
}

void demoSinglyLinkedListReversePairs() {
    std::cout << "--- SinglyLinkedList: reversePairs() ---\n";

    SinglyLinkedList evenList;
    for (int i = 1; i <= 6; i++) {
        evenList.append(makeRecord(i));
    }
    std::cout << "Even-length list (6 elements) before:\n";
    evenList.display();
    evenList.reversePairs();
    std::cout << "After reversePairs (expected: 2,1,4,3,6,5):\n";
    evenList.display();

    std::cout << "\n";

    SinglyLinkedList oddList;
    for (int i = 1; i <= 7; i++) {
        oddList.append(makeRecord(i));
    }
    std::cout << "Odd-length list (7 elements) before:\n";
    oddList.display();
    oddList.reversePairs();
    std::cout << "After reversePairs (expected: 2,1,4,3,6,5,7 -- trailing 7 stays in place):\n";
    oddList.display();
    std::cout << "\n";
}

}  // namespace

int main() {
    demoDoublyLinkedList();
    demoSinglyLinkedListReverse();
    demoSinglyLinkedListReversePairs();
    return 0;
}
