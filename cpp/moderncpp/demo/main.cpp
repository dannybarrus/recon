#include <chrono>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "recon/moderncpp/Buffer.h"
#include "recon/moderncpp/Clamp.h"
#include "recon/moderncpp/Describe.h"
#include "recon/moderncpp/GenericStack.h"
#include "recon/moderncpp/MoveSemanticsExamples.h"
#include "recon/moderncpp/Pair.h"
#include "recon/moderncpp/TypeBranching.h"
#include "recon/moderncpp/VariadicSum.h"

using namespace recon::moderncpp;

namespace {

void demoMoveIsJustACast() {
    std::cout << "--- std::move is just a cast, not an action ---\n";

    MoveCastResult result = demonstrateMoveIsJustACast("hello world");

    std::cout << "  value right after the std::move() cast: \"" << result.valueRightAfterCast
              << "\"   (expected: \"hello world\" -- the cast alone did nothing)\n";
    std::cout << "  original AFTER an actual move-construction consumed it: \""
              << result.originalAfterRealMove << "\"   (now in a moved-from state)\n";
    std::cout << "  the value that received the move: \"" << result.movedToValue << "\"\n\n";
}

void demoOverloadResolution() {
    std::cout << "--- lvalue vs rvalue overload resolution ---\n";

    std::string named = "named variable";
    std::cout << "  " << whichOverload(named) << "\n";
    std::cout << "  " << whichOverload(std::string("temporary")) << "\n";
    std::cout << "  " << whichOverload(std::move(named)) << "\n";
    std::cout << "  (std::move(named) is what makes 'named' eligible to bind to the\n";
    std::cout << "   rvalue-reference overload -- without it, 'named' would bind to\n";
    std::cout << "   the lvalue overload, same as the first call above.)\n\n";
}

template <typename... Args>
Buffer makeBufferForwarding(Args&&... args) {
    return Buffer(std::forward<Args>(args)...);
}

void demoPerfectForwarding() {
    std::cout << "--- perfect forwarding ---\n";
    Buffer::resetCounts();

    std::size_t requestedSize = 100;
    Buffer forwarded = makeBufferForwarding(requestedSize);

    std::cout << "  forwarded Buffer size: " << forwarded.size() << "   (expected: 100)\n";
    std::cout << "  copies performed during construction: " << Buffer::copyCount()
              << "   (expected: 0 -- the argument itself was just a size_t, not a Buffer)\n\n";
}

void demoMoveSemanticsBenchmark() {
    std::cout << "--- move vs copy: an actual measured performance comparison ---\n";

    constexpr std::size_t kBufferSize = 500000;  // 500K doubles = 4MB per buffer
    constexpr int kBufferCount = 200;

    // Pre-construct sources for the COPY benchmark, run it, then free
    // them before building sources for the MOVE benchmark. This keeps
    // peak memory well under 1GB regardless of what machine this runs
    // on -- holding both sets of sources AND a growing destination
    // vector simultaneously (200 * 4MB * 3 = ~2.4GB) was the cause of
    // an earlier out-of-memory kill of this exact benchmark in a
    // memory-constrained environment. Building, timing, and freeing
    // one phase completely before starting the next avoids that.
    std::vector<Buffer> copySources;
    copySources.reserve(kBufferCount);
    for (int i = 0; i < kBufferCount; i++) {
        copySources.emplace_back(kBufferSize);
    }

    Buffer::resetCounts();
    auto copyStart = std::chrono::steady_clock::now();
    {
        std::vector<Buffer> copies;
        copies.reserve(kBufferCount);
        for (int i = 0; i < kBufferCount; i++) {
            copies.push_back(copySources[i]);  // copy-constructs -- this is ALL that's timed
        }
    }
    auto copyEnd = std::chrono::steady_clock::now();
    auto copyMs = std::chrono::duration_cast<std::chrono::microseconds>(copyEnd - copyStart).count();
    int copiesPerformed = Buffer::copyCount();
    copySources.clear();  // free before building the next phase's sources

    std::vector<Buffer> moveSources;
    moveSources.reserve(kBufferCount);
    for (int i = 0; i < kBufferCount; i++) {
        moveSources.emplace_back(kBufferSize);
    }

    Buffer::resetCounts();
    auto moveStart = std::chrono::steady_clock::now();
    {
        std::vector<Buffer> moved;
        moved.reserve(kBufferCount);
        for (int i = 0; i < kBufferCount; i++) {
            moved.push_back(std::move(moveSources[i]));  // move-constructs -- this is ALL that's timed
        }
    }
    auto moveEnd = std::chrono::steady_clock::now();
    auto moveMs = std::chrono::duration_cast<std::chrono::microseconds>(moveEnd - moveStart).count();
    int movesPerformed = Buffer::moveCount();

    std::cout << "  " << kBufferCount << " buffers of " << kBufferSize << " doubles each (4MB each):\n";
    std::cout << "    copy-constructing all " << kBufferCount << ": " << copyMs << " microseconds"
              << "  (copyCount = " << copiesPerformed << ")\n";
    std::cout << "    move-constructing all " << kBufferCount << ": " << moveMs << " microseconds"
              << "  (moveCount = " << movesPerformed << ")\n";
    std::cout << "  Each phase builds its own sources, times ONLY the copy or move\n";
    std::cout << "  operation itself, then frees that phase's memory before the next\n";
    std::cout << "  phase starts. Copying does real O(size) work every time -- allocate\n";
    std::cout << "  plus copy every element. Moving is O(1) every time -- steal a\n";
    std::cout << "  pointer, done.\n\n";
}

void demoCopyElisionNote() {
    std::cout << "--- copy elision vs move: a commonly conflated pair ---\n";
    std::cout << "  Since C++17, returning a temporary by value (a 'prvalue') from a\n";
    std::cout << "  function is GUARANTEED to be elided entirely -- no copy AND no move\n";
    std::cout << "  constructor runs at all; the object is constructed directly in its\n";
    std::cout << "  final location. This is mandatory copy elision, not move semantics,\n";
    std::cout << "  even though people often describe it as 'the move constructor ran.'\n";

    Buffer::resetCounts();
    Buffer b = Buffer(50);
    std::cout << "  moveCount after 'Buffer b = Buffer(50);': " << Buffer::moveCount()
              << "   (expected: 0 -- elision means there was nothing to move FROM)\n\n";
}

void demoClassTemplate_Pair() {
    std::cout << "--- Pair<T1, T2>: a class template ---\n";

    Pair<int, std::string> p1(42, "answer");
    std::cout << "  Pair<int, std::string>: (" << p1.first() << ", " << p1.second() << ")\n";

    auto p2 = makePair(3.14, true);
    std::cout << "  makePair(3.14, true) deduced Pair<double, bool>: ("
              << p2.first() << ", " << p2.second() << ")\n\n";
}

void demoClassTemplate_GenericStack() {
    std::cout << "--- GenericStack<T>: the deferred genericity payoff ---\n";
    std::cout << "  datastructures/SinglyLinkedList and DoublyLinkedList were written\n";
    std::cout << "  specifically for PhoneRecord, on purpose -- genericity was deferred\n";
    std::cout << "  to this module. Here it is, written exactly once for any type:\n\n";

    GenericStack<int> intStack;
    intStack.push(1);
    intStack.push(2);
    intStack.push(3);
    std::cout << "  GenericStack<int>: top=" << intStack.top() << ", size=" << intStack.size() << "\n";

    GenericStack<std::string> stringStack;
    stringStack.push("a");
    stringStack.push("b");
    std::cout << "  GenericStack<std::string>: top=\"" << stringStack.top()
              << "\", size=" << stringStack.size() << "\n\n";
}

void demoFunctionTemplate_Clamp() {
    std::cout << "--- clamp<T>: a function template ---\n";
    std::cout << "  clamp(15, 0, 10) = " << clamp(15, 0, 10) << "   (expected: 10)\n";
    std::cout << "  clamp(-5, 0, 10) = " << clamp(-5, 0, 10) << "   (expected: 0)\n";
    std::cout << "  clamp(2.5, 0.0, 10.0) = " << clamp(2.5, 0.0, 10.0) << "   (expected: 2.5)\n\n";
}

void demoTemplateSpecialization_Describe() {
    std::cout << "--- describe<T>: explicit template specialization ---\n";
    std::cout << "  describe(42) = \"" << describe(42) << "\"\n";
    std::cout << "  describe(3.14) = \"" << describe(3.14) << "\"\n";
    std::cout << "  describe(true) = \"" << describe(true) << "\"   (specialized -- not \"1\")\n";
    std::cout << "  describe(false) = \"" << describe(false) << "\"   (specialized -- not \"0\")\n\n";
}

void demoVariadicTemplates() {
    std::cout << "--- variadic templates: recursive unpacking vs fold expressions ---\n";
    std::cout << "  sumRecursive(1, 2, 3, 4) = " << sumRecursive(1, 2, 3, 4) << "   (expected: 10)\n";
    std::cout << "  sumFold(1, 2, 3, 4)      = " << sumFold(1, 2, 3, 4) << "   (expected: 10, same result)\n";
    std::cout << "  sumFold(1.5, 2.5, 3.0)   = " << sumFold(1.5, 2.5, 3.0) << "   (expected: 7)\n\n";
}

void demoIfConstexpr() {
    std::cout << "--- if constexpr: compile-time branching by type ---\n";

    int value = 42;
    int* ptr = &value;
    int* nullPtr = nullptr;
    std::string text = "hello";

    std::cout << "  describeKind(42)       -> " << describeKind(value) << "\n";
    std::cout << "  describeKind(&value)   -> " << describeKind(ptr) << "\n";
    std::cout << "  describeKind(nullPtr)  -> " << describeKind(nullPtr) << "\n";
    std::cout << "  describeKind(\"hello\")  -> " << describeKind(text) << "\n\n";
}

}  // namespace

int main() {
    demoMoveIsJustACast();
    demoOverloadResolution();
    demoPerfectForwarding();
    demoMoveSemanticsBenchmark();
    demoCopyElisionNote();
    demoClassTemplate_Pair();
    demoClassTemplate_GenericStack();
    demoFunctionTemplate_Clamp();
    demoTemplateSpecialization_Describe();
    demoVariadicTemplates();
    demoIfConstexpr();
    return 0;
}
