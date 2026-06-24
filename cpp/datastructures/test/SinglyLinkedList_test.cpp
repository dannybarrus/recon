#include "recon/datastructures/SinglyLinkedList.h"

#include <algorithm>
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

namespace recon::datastructures {
namespace {

std::string captureDisplay(const SinglyLinkedList& list) {
    std::ostringstream captured;
    std::streambuf* original = std::cout.rdbuf(captured.rdbuf());
    list.display();
    std::cout.rdbuf(original);
    return captured.str();
}

std::vector<std::string> namesInOrder(const std::string& output,
                                       const std::vector<std::string>& candidates) {
    std::vector<std::pair<std::size_t, std::string>> found;
    for (const auto& name : candidates) {
        auto pos = output.find(name);
        if (pos != std::string::npos) {
            found.push_back({pos, name});
        }
    }
    std::sort(found.begin(), found.end());
    std::vector<std::string> result;
    for (auto& p : found) result.push_back(p.second);
    return result;
}

SinglyLinkedList buildList(const std::vector<std::string>& names) {
    SinglyLinkedList list;
    for (const auto& name : names) {
        list.append({name, "0"});
    }
    return list;
}

TEST(SinglyLinkedListTest, EmptyList_HasZeroSize) {
    SinglyLinkedList list;
    EXPECT_EQ(list.size(), 0u);
    EXPECT_TRUE(list.empty());
}

TEST(SinglyLinkedListTest, Append_BuildsInOrder) {
    SinglyLinkedList list = buildList({"A", "B", "C"});
    std::string output = captureDisplay(list);
    auto order = namesInOrder(output, {"A", "B", "C"});
    EXPECT_EQ(order, (std::vector<std::string>{"A", "B", "C"}));
    EXPECT_EQ(list.size(), 3u);
}

TEST(SinglyLinkedListTest, Reverse_FiveElements) {
    SinglyLinkedList list = buildList({"A", "B", "C", "D", "E"});
    list.reverse();
    std::string output = captureDisplay(list);
    auto order = namesInOrder(output, {"A", "B", "C", "D", "E"});
    EXPECT_EQ(order, (std::vector<std::string>{"E", "D", "C", "B", "A"}));
}

TEST(SinglyLinkedListTest, Reverse_EmptyList_DoesNotCrash) {
    SinglyLinkedList list;
    list.reverse();
    EXPECT_TRUE(list.empty());
}

TEST(SinglyLinkedListTest, Reverse_SingleElement_Unchanged) {
    SinglyLinkedList list = buildList({"Solo"});
    list.reverse();
    std::string output = captureDisplay(list);
    EXPECT_NE(output.find("Solo"), std::string::npos);
    EXPECT_EQ(list.size(), 1u);
}

TEST(SinglyLinkedListTest, ReversePairs_EvenLength) {
    SinglyLinkedList list = buildList({"N1", "N2", "N3", "N4", "N5", "N6"});
    list.reversePairs();
    std::string output = captureDisplay(list);
    auto order = namesInOrder(output, {"N1", "N2", "N3", "N4", "N5", "N6"});
    EXPECT_EQ(order, (std::vector<std::string>{"N2", "N1", "N4", "N3", "N6", "N5"}));
}

TEST(SinglyLinkedListTest, ReversePairs_OddLength_TrailingElementStaysInPlace) {
    // [1,2,3,4,5,6,7] -> [2,1,4,3,6,5,7] -- the verified-correct result
    // from the original lnklstrevpairs.cpp algorithm.
    SinglyLinkedList list = buildList({"N1", "N2", "N3", "N4", "N5", "N6", "N7"});
    list.reversePairs();
    std::string output = captureDisplay(list);
    auto order = namesInOrder(output, {"N1", "N2", "N3", "N4", "N5", "N6", "N7"});
    EXPECT_EQ(order, (std::vector<std::string>{"N2", "N1", "N4", "N3", "N6", "N5", "N7"}));
}

TEST(SinglyLinkedListTest, ReversePairs_TwoElements) {
    SinglyLinkedList list = buildList({"A", "B"});
    list.reversePairs();
    std::string output = captureDisplay(list);
    auto order = namesInOrder(output, {"A", "B"});
    EXPECT_EQ(order, (std::vector<std::string>{"B", "A"}));
}

TEST(SinglyLinkedListTest, ReversePairs_SingleElement_Unchanged) {
    SinglyLinkedList list = buildList({"Solo"});
    list.reversePairs();
    std::string output = captureDisplay(list);
    EXPECT_NE(output.find("Solo"), std::string::npos);
    EXPECT_EQ(list.size(), 1u);
}

TEST(SinglyLinkedListTest, ReversePairs_EmptyList_DoesNotCrash) {
    SinglyLinkedList list;
    list.reversePairs();
    EXPECT_TRUE(list.empty());
}

TEST(SinglyLinkedListTest, ReversePairs_PreservesTotalCount) {
    SinglyLinkedList list = buildList({"A", "B", "C", "D", "E", "F", "G"});
    list.reversePairs();
    EXPECT_EQ(list.size(), 7u);
}

TEST(SinglyLinkedListTest, MoveConstruction_TransfersOwnership) {
    SinglyLinkedList original = buildList({"A", "B", "C"});
    SinglyLinkedList moved(std::move(original));

    EXPECT_EQ(moved.size(), 3u);
    EXPECT_TRUE(original.empty());  // moved-from list is left empty, not dangling
}

TEST(SinglyLinkedListTest, MoveAssignment_TransfersOwnershipAndFreesOldData) {
    SinglyLinkedList target = buildList({"X", "Y"});
    SinglyLinkedList source = buildList({"A", "B", "C"});

    target = std::move(source);

    EXPECT_EQ(target.size(), 3u);
    EXPECT_TRUE(source.empty());
}

}  // namespace
}  // namespace recon::datastructures
