#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

#include "recon/memory/ExceptionSafetyExamples.h"
#include "recon/memory/GraphNode.h"
#include "recon/memory/LegacyHandle.h"
#include "recon/memory/ScopedLock.h"
#include "recon/memory/TrackedResource.h"

using namespace recon::memory;

namespace {

void demoRaiiExceptionSafety() {
    std::cout << "--- RAII vs manual cleanup, under an exception ---\n";

    TrackedResource::resetAliveCount();
    try {
        leaksOnException_rawPointer(true);
    } catch (const std::exception&) {
    }
    std::cout << "  After raw-pointer path threw: aliveCount = " << TrackedResource::aliveCount()
              << "   (LEAKED -- the delete was never reached)\n";

    TrackedResource::resetAliveCount();
    try {
        doesNotLeakOnException_uniquePtr(true);
    } catch (const std::exception&) {
    }
    std::cout << "  After unique_ptr path threw:  aliveCount = " << TrackedResource::aliveCount()
              << "   (expected: 0 -- destructor ran during stack unwinding)\n\n";
}

void demoUniquePtrBasics() {
    std::cout << "--- unique_ptr: basic ownership, no delete written anywhere ---\n";

    TrackedResource::resetAliveCount();
    {
        auto ptr = std::make_unique<TrackedResource>("scoped");
        std::cout << "  inside scope:  aliveCount = " << TrackedResource::aliveCount() << "\n";
    }
    std::cout << "  after scope exit: aliveCount = " << TrackedResource::aliveCount()
              << "   (expected: 0)\n\n";
}

void demoUniquePtrMoveOnly() {
    std::cout << "--- unique_ptr: move-only, for free ---\n";
    std::cout << "  datastructures/SinglyLinkedList had to manually write\n";
    std::cout << "  '= delete' for copy and a hand-written move constructor.\n";
    std::cout << "  unique_ptr gives every class that contains one the same\n";
    std::cout << "  property automatically -- no extra code required.\n\n";

    TrackedResource::resetAliveCount();
    auto original = std::make_unique<TrackedResource>("movable");
    auto moved = std::move(original);

    std::cout << "  original owns it after move: " << (original != nullptr ? "yes" : "no") << "\n";
    std::cout << "  moved owns it after move:    " << (moved != nullptr ? "yes" : "no") << "\n";
    std::cout << "  aliveCount throughout: " << TrackedResource::aliveCount()
              << "   (expected: 1, unchanged -- only ownership moved, nothing new/deleted)\n\n";
}

void demoUniquePtrCustomDeleter() {
    std::cout << "--- unique_ptr with a custom deleter, wrapping a C-style API ---\n";

    LegacyHandle::resetAliveCount();
    {
        std::unique_ptr<LegacyHandle, decltype(&LegacyHandle::close)> handle(
            LegacyHandle::open("config.dat"), &LegacyHandle::close);
        std::cout << "  using handle for: " << handle->name() << "\n";
        std::cout << "  aliveCount while in use: " << LegacyHandle::aliveCount() << "\n";
    }
    std::cout << "  aliveCount after scope exit: " << LegacyHandle::aliveCount()
              << "   (expected: 0 -- close() was called automatically)\n\n";
}

void demoMakeUniqueVsNew() {
    std::cout << "--- make_unique/make_shared vs raw new ---\n";
    std::cout << "  Prefer make_unique<T>(args) / make_shared<T>(args) over\n";
    std::cout << "  unique_ptr<T>(new T(args)). Reasons that still apply under\n";
    std::cout << "  C++17 (this project's standard):\n";
    std::cout << "    1. Avoids repeating the type name.\n";
    std::cout << "    2. make_shared does ONE combined allocation for the object\n";
    std::cout << "       and its control block, instead of two separate ones.\n";
    std::cout << "  Historical note: pre-C++17, 'f(shared_ptr<T>(new T), g())'\n";
    std::cout << "  could leak T if g() threw between the new and the shared_ptr\n";
    std::cout << "  taking ownership -- argument evaluation order wasn't\n";
    std::cout << "  sequenced strictly enough to prevent it. C++17 closed that\n";
    std::cout << "  specific gap, but make_shared's allocation win remains a\n";
    std::cout << "  good reason to prefer it regardless.\n\n";
}

void demoSharedPtrCycle_TheLeak() {
    std::cout << "--- shared_ptr: the circular-reference leak ---\n";

    GraphNode::resetAliveCount();
    {
        auto a = std::make_shared<GraphNode>("A");
        auto b = std::make_shared<GraphNode>("B");
        a->next = b;
        b->next = a;
        std::cout << "  inside scope: aliveCount = " << GraphNode::aliveCount() << "\n";
    }
    std::cout << "  after scope exit: aliveCount = " << GraphNode::aliveCount()
              << "   (LEAKED -- expected 0 if there were no cycle; neither\n";
    std::cout << "   destructor ran, because each node's refcount still includes\n";
    std::cout << "   the reference held by the OTHER node in the cycle)\n\n";
}

void demoSharedPtrCycle_FixedWithWeakPtr() {
    std::cout << "--- shared_ptr cycle, fixed with weak_ptr ---\n";

    GraphNode::resetAliveCount();
    {
        auto a = std::make_shared<GraphNode>("A");
        auto b = std::make_shared<GraphNode>("B");
        a->next = b;
        b->weakNext = a;
        std::cout << "  inside scope: aliveCount = " << GraphNode::aliveCount() << "\n";
    }
    std::cout << "  after scope exit: aliveCount = " << GraphNode::aliveCount()
              << "   (expected: 0 -- the cycle is broken, both destructors ran)\n\n";
}

void demoScopedLock() {
    std::cout << "--- ScopedLock: correctness under real concurrent contention ---\n";

    std::mutex mtx;
    int sharedCounter = 0;
    constexpr int kIncrementsPerThread = 100000;

    auto increment = [&]() {
        for (int i = 0; i < kIncrementsPerThread; i++) {
            ScopedLock<std::mutex> lock(mtx);
            sharedCounter++;
        }
    };

    std::thread t1(increment);
    std::thread t2(increment);
    t1.join();
    t2.join();

    std::cout << "  sharedCounter = " << sharedCounter
              << "   (expected: " << (2 * kIncrementsPerThread) << ")\n";
    std::cout << "  No unlock() call appears anywhere in increment() -- the\n";
    std::cout << "  destructor handles it, on every code path.\n\n";
}

void demoScopedLockExceptionSafety() {
    std::cout << "--- ScopedLock: the lock is released even when an exception is thrown ---\n";

    std::mutex mtx;
    try {
        ScopedLock<std::mutex> lock(mtx);
        throw std::runtime_error("simulated failure while holding the lock");
    } catch (const std::exception& e) {
        std::cout << "  caught: " << e.what() << "\n";
    }

    bool acquired = mtx.try_lock();
    std::cout << "  lock successfully re-acquired after the exception: " << (acquired ? "yes" : "no")
              << "   (expected: yes -- if the destructor hadn't run, this would fail)\n";
    if (acquired) {
        mtx.unlock();
    }
}

}  // namespace

int main() {
    demoRaiiExceptionSafety();
    demoUniquePtrBasics();
    demoUniquePtrMoveOnly();
    demoUniquePtrCustomDeleter();
    demoMakeUniqueVsNew();
    demoSharedPtrCycle_TheLeak();
    demoSharedPtrCycle_FixedWithWeakPtr();
    demoScopedLock();
    demoScopedLockExceptionSafety();
    return 0;
}
