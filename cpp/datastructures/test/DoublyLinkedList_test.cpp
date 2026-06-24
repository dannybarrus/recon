#include "recon/datastructures/DoublyLinkedList.h"

#include <gtest/gtest.h>
#include <sstream>

#include <iostream>

namespace recon::datastructures {
namespace {

std::string captureDisplay(const DoublyLinkedList& list) {
    std::ostringstream captured;
    std::streambuf* original = std::cout.rdbuf(captured.rdbuf());
    list.display();
    std::cout.rdbuf(original);
    return captured.str();
}

TEST(DoublyLinkedListTest, EmptyList_HasZeroSize) {
    DoublyLinkedList list;
    EXPECT_EQ(list.size(), 0u);
    EXPECT_TRUE(list.empty());
}

TEST(DoublyLinkedListTest, SingleInsert) {
    DoublyLinkedList list;
    list.insertSorted({"Alice", "555-0101"});
    EXPECT_EQ(list.size(), 1u);
    EXPECT_FALSE(list.empty());
}

TEST(DoublyLinkedListTest, InsertsInOrder_RemainSorted) {
    DoublyLinkedList list;
    list.insertSorted({"Alice", "1"});
    list.insertSorted({"Bob", "2"});
    list.insertSorted({"Charlie", "3"});

    std::string output = captureDisplay(list);
    EXPECT_NE(output.find("Alice"), std::string::npos);
    auto aliceIdx = output.find("Alice");
    auto bobIdx = output.find("Bob");
    auto charlieIdx = output.find("Charlie");
    EXPECT_LT(aliceIdx, bobIdx);
    EXPECT_LT(bobIdx, charlieIdx);
}

TEST(DoublyLinkedListTest, InsertsOutOfOrder_EndUpSorted) {
    // Regression test for the original's pPrev bug -- inserts records
    // in a scrambled order, exactly the scenario the bug needed to
    // manifest.
    DoublyLinkedList list;
    list.insertSorted({"Charlie", "3"});
    list.insertSorted({"Alice", "1"});
    list.insertSorted({"Echo", "5"});
    list.insertSorted({"Bob", "2"});
    list.insertSorted({"Delta", "4"});

    std::string output = captureDisplay(list);
    auto aliceIdx = output.find("Alice");
    auto bobIdx = output.find("Bob");
    auto charlieIdx = output.find("Charlie");
    auto deltaIdx = output.find("Delta");
    auto echoIdx = output.find("Echo");

    EXPECT_LT(aliceIdx, bobIdx);
    EXPECT_LT(bobIdx, charlieIdx);
    EXPECT_LT(charlieIdx, deltaIdx);
    EXPECT_LT(deltaIdx, echoIdx);
}

TEST(DoublyLinkedListTest, InsertBeforeHead_BecomesNewHead) {
    DoublyLinkedList list;
    list.insertSorted({"Bob", "2"});
    list.insertSorted({"Alice", "1"});

    std::string output = captureDisplay(list);
    EXPECT_LT(output.find("Alice"), output.find("Bob"));
}

TEST(DoublyLinkedListTest, Size_TracksMultipleInserts) {
    DoublyLinkedList list;
    for (int i = 0; i < 10; i++) {
        list.insertSorted({"Name" + std::to_string(i), "0"});
    }
    EXPECT_EQ(list.size(), 10u);
}

}  // namespace
}  // namespace recon::datastructures
