#include "recon/moderncpp/MoveSemanticsExamples.h"

#include <gtest/gtest.h>
#include <utility>

namespace recon::moderncpp {
namespace {

TEST(MoveSemanticsExamplesTest, MoveCast_DoesNotChangeValueByItself) {
    MoveCastResult result = demonstrateMoveIsJustACast("hello world");
    EXPECT_EQ(result.valueRightAfterCast, "hello world");
}

TEST(MoveSemanticsExamplesTest, ActualMoveConstruction_TransfersTheValue) {
    MoveCastResult result = demonstrateMoveIsJustACast("hello world");
    EXPECT_EQ(result.movedToValue, "hello world");
}

TEST(MoveSemanticsExamplesTest, ActualMoveConstruction_LeavesSourceMovedFrom) {
    MoveCastResult result = demonstrateMoveIsJustACast("hello world");
    // Per the header's documentation: the standard only guarantees a
    // moved-from std::string is left in SOME valid state, not
    // specifically empty. On this standard library implementation, it
    // is empty in practice -- this test pins that observed behavior so
    // a future standard library change would be caught, rather than
    // silently assuming it without verification.
    EXPECT_EQ(result.originalAfterRealMove, "");
}

TEST(WhichOverloadTest, NamedVariable_CallsLvalueOverload) {
    std::string named = "value";
    std::string result = whichOverload(named);
    EXPECT_NE(result.find("lvalue overload"), std::string::npos);
}

TEST(WhichOverloadTest, Temporary_CallsRvalueOverload) {
    std::string result = whichOverload(std::string("temporary"));
    EXPECT_NE(result.find("rvalue overload"), std::string::npos);
}

TEST(WhichOverloadTest, StdMove_MakesNamedVariableCallRvalueOverload) {
    std::string named = "value";
    std::string result = whichOverload(std::move(named));
    EXPECT_NE(result.find("rvalue overload"), std::string::npos);
}

}  // namespace
}  // namespace recon::moderncpp
