/** Locks
 * 
 * Locks are available in 4 different flavours:
 * - std::lock_guard
 * - std::unique_lock
 * - std::shared_lock (since C++14)
 * - std::scoped_lock (since C++17)
 * 
 * ======================== std::lock_guard
 * The lifetime of std::lock_guard is limited by its scope and the scope is defined
 * by the curly brackets. This means that its lifetime ends when it passes the closing
 * curly brackets. Exactly then, the std::lock_guard destructor is called and the mutex
 * is released. This happens automatically and, it happens also if `get_var()` in 
 * `lock_guard()` function throws an exception. A function scope and loop scope also
 * limit the lifetime of an object.
 * 
 * ======================== std::unique_lock
 * A std::unique_lock is stronger but more expensive than std::lock_guard. In addition
 * to what's offered by a std::lock_guard, a std::unique_lock enables you to
 * - create it without an associated mutex
 * - create it without locking the associated mutex
 * - explicitly and repeately set or release the lock of the associated mutex
 * - recursively lock its mutex
 * - move the mutex
 * - try to lock the mutex
 * - delay the lock on the associated mutex
 * 
 * ======================== std::shared_lock (since C++14)
 * A std::shared_lock has the same interface as a std::unique_lock but behaves differently
 * when used with a std::shared_timed_mutex or a std::shared_mutex. Many threads can share
 * one std::shared_timed_mutex(std::shared_mutex) and, therefore, implement a reader-writer
 * lock. The idea of reader-writer locks is straightforward and extremely useful. An arbitrary
 * number of threads executing read operations can access the critical regison at the same time,
 * but only one thread is allowed to write.
 * 
 * Reader-write locks do not solve the fundamental problem - threads competing for access to
 * a critical region, but they do help to minimize the bottleneck.
 * 
 * - refer 'practice-cpp/shared_lock' for an example.
 * 
 * ======================== std::scoped_lock (since C++17)
 * It's very similar to std::lock_guard, but std::scoped_lock can, additionally, lock an
 * arbitrary number of mutexes atomically. You have to keep a few facts in mind.
 * 
 * 1. If std::scoped_lock is invoked with one mutex `m` it behaves such as a std::lock_guard
 * and locks the mutex `m`(m.lock()). If the std::scoped_lock is invoked with more than one
 * mutex (std::scoped_lock(MutexType&&... m)), it uses the function std::lock(m...).
 * 2. If one of the current threads already owns  the corresponding mutex and the mutex is
 * not recursive, the behavior is undefined. With high probability, you get a deadlock.
 * 3. You can just take the onwership of the mutex without locking them. In this case, you
 * have to provide the std::adopt_lock_t flag to the constructor: std::scoped_lock(std::adopt_lock_t,
 * MutexTypes&... m).
 * 
 * By using a std::scoped_lock, you can quite elegantly solve the deadlock.
 */

#include <iostream>
#include <mutex>

#include "unique_lock.h"
#include "scoped_lock.h"

int shared_var = 0;
int get_var() { return 10; }

void lock_guard()
{
    {
        std::mutex m;
        std::lock_guard<std::mutex> lock(m);
        shared_var = get_var();
    }
}

int main()
{
    unique_lock::test_dead_lock();
    std::cout << "-------------------------------------------------------" << std::endl << std::endl;
    unique_lock::test_dead_lock2();
    std::cout << "-------------------------------------------------------" << std::endl << std::endl;
    scoped_lock::test_dead_lock();
    std::cout << "-------------------------------------------------------" << std::endl << std::endl;
}