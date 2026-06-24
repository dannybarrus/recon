#include "recon/memory/ScopedLock.h"

#include <gtest/gtest.h>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

namespace recon::memory {
namespace {

TEST(ScopedLockTest, LocksOnConstruction_UnlocksOnDestruction) {
    std::mutex mtx;
    {
        ScopedLock<std::mutex> lock(mtx);
        (void)lock;
    }
    EXPECT_TRUE(mtx.try_lock());
    mtx.unlock();
}

TEST(ScopedLockTest, ReleasesLockEvenWhenExceptionIsThrown) {
    std::mutex mtx;

    EXPECT_THROW(
        {
            ScopedLock<std::mutex> lock(mtx);
            throw std::runtime_error("simulated failure while holding the lock");
        },
        std::runtime_error);

    // The actual point of the test: if the destructor had NOT run
    // during stack unwinding, this would fail.
    EXPECT_TRUE(mtx.try_lock());
    mtx.unlock();
}

TEST(ScopedLockTest, EnsuresMutualExclusion_UnderRealConcurrency) {
    std::mutex mtx;
    int counter = 0;
    constexpr int kThreadCount = 8;
    constexpr int kIncrementsPerThread = 50000;

    auto increment = [&]() {
        for (int i = 0; i < kIncrementsPerThread; i++) {
            ScopedLock<std::mutex> lock(mtx);
            counter++;
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < kThreadCount; i++) {
        threads.emplace_back(increment);
    }
    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(counter, kThreadCount * kIncrementsPerThread);
}

}  // namespace
}  // namespace recon::memory
