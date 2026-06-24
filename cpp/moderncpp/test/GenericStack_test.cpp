#include "recon/moderncpp/GenericStack.h"

#include <gtest/gtest.h>
#include <stdexcept>
#include <string>

namespace recon::moderncpp {
namespace {

TEST(GenericStackTest, NewStack_IsEmpty) {
    GenericStack<int> stack;
    EXPECT_TRUE(stack.empty());
    EXPECT_EQ(stack.size(), 0u);
}

TEST(GenericStackTest, PushThenTop_ReturnsLastPushed) {
    GenericStack<int> stack;
    stack.push(1);
    stack.push(2);
    stack.push(3);
    EXPECT_EQ(stack.top(), 3);
    EXPECT_EQ(stack.size(), 3u);
}

TEST(GenericStackTest, Pop_RemovesLastPushed) {
    GenericStack<int> stack;
    stack.push(1);
    stack.push(2);
    stack.pop();
    EXPECT_EQ(stack.top(), 1);
    EXPECT_EQ(stack.size(), 1u);
}

TEST(GenericStackTest, Pop_OnEmptyStack_Throws) {
    GenericStack<int> stack;
    EXPECT_THROW(stack.pop(), std::out_of_range);
}

TEST(GenericStackTest, Top_OnEmptyStack_Throws) {
    GenericStack<int> stack;
    EXPECT_THROW(stack.top(), std::out_of_range);
}

TEST(GenericStackTest, WorksWithStringType) {
    GenericStack<std::string> stack;
    stack.push("first");
    stack.push("second");
    EXPECT_EQ(stack.top(), "second");
    stack.pop();
    EXPECT_EQ(stack.top(), "first");
}

TEST(GenericStackTest, WorksWithCustomStruct) {
    struct Point {
        int x;
        int y;
    };
    GenericStack<Point> stack;
    stack.push(Point{1, 2});
    stack.push(Point{3, 4});
    EXPECT_EQ(stack.top().x, 3);
    EXPECT_EQ(stack.top().y, 4);
}

}  // namespace
}  // namespace recon::moderncpp
