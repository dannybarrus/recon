#pragma once

namespace recon::memory {

// A minimal RAII lock guard -- acquire the lock in the constructor,
// release it in the destructor. This is the standard library's own
// std::lock_guard, reimplemented here specifically to make the
// mechanism visible rather than reaching for the pre-built version
// immediately.
//
// The direct payoff this demonstrates: in concurrency/'s
// BankAccountWithLock (the Java module's ReentrantLock equivalent),
// every single method had to wrap its critical section in a manual
// try { ... } finally { lock.unlock(); } block -- a DISCIPLINE the
// programmer has to remember and apply correctly every single time a
// lock is taken, anywhere in the codebase. Forgetting the finally
// block, or putting code after the unlock that should have been
// inside it, are both real, easy mistakes.
//
// ScopedLock needs no such discipline. The lock is released by the
// destructor, which C++ guarantees runs when the ScopedLock variable
// goes out of scope -- on a normal return, an early return, a break,
// or an exception propagating through. There is no unlock() call
// anywhere in code that uses this class, and therefore no way to
// forget one.
//
// Templated on the mutex type so it works with std::mutex,
// std::recursive_mutex, or any other type exposing lock()/unlock().
template <typename Mutex>
class ScopedLock {
public:
    explicit ScopedLock(Mutex& mutex) : mutex_(mutex) {
        mutex_.lock();
    }

    ~ScopedLock() {
        mutex_.unlock();
    }

    ScopedLock(const ScopedLock&) = delete;
    ScopedLock& operator=(const ScopedLock&) = delete;

private:
    Mutex& mutex_;
};

}  // namespace recon::memory
