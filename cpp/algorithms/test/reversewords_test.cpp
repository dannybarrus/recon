#include "recon/algorithms/reversewords.h"

#include <gtest/gtest.h>

namespace recon::algorithms {
namespace {

TEST(ReverseWordsTest, SimpleSentence) {
    char buf[] = "This is a test";
    reverseWords(buf);
    EXPECT_STREQ(buf, "test a is This");
}

TEST(ReverseWordsTest, SingleWord) {
    char buf[] = "hello";
    reverseWords(buf);
    EXPECT_STREQ(buf, "hello");
}

TEST(ReverseWordsTest, TwoWords) {
    char buf[] = "hello world";
    reverseWords(buf);
    EXPECT_STREQ(buf, "world hello");
}

TEST(ReverseWordsTest, EmptyString) {
    char buf[] = "";
    reverseWords(buf);
    EXPECT_STREQ(buf, "");
}

}  // namespace
}  // namespace recon::algorithms
