/** The following table shows the methods of a std::unique_lock `lk`.

| Method                        | Description
------------------------------------------------------------------------------
| lk.lock()                     | Locks the associated mutex.
| lk.try_lock()                 | Tries to lock the associated mutex.
| lk.try_lock_for(rel_time)     |
| lk.try_lock_until(abs_time)   |
| lk.unlock()                   | Unlocks the associated mutex.
| lk.release()                  | Release the mutex. The mutex remains locked.
| lk.swap(lk2)                  | Swaps the locks.
| std::swap(lk, lk2)            |
| lk.mutex()                    | Returns a pointer to the associated mutex.
| lk.owns_lock()                | Checks if the lock `lk` has a locked mutex.
| operator bool                 |
------------------------------------------------------------------------------
 * lk.try_lock_for(rel_time) needs a relative time duration and lk.try_lock_until(abs_time)
 * needs an absolute time point.
 * 
 * lk.try_lock tries to lock the mutex and returns immediately. On success, it return true,
 * otherwise false. In contrast, the methods lk.try_lock_for and lk.try_lock_until the lock
 * `lk` blocks until the specified timeout occurs or the lock is acquired, whichever comes
 * first. All three methods lk.try_lock, lk.try_lock_for, and lk.try_lock_until throw a
 * std::system_error exception if there is no associated mutex of if the mutex is already
 * locked by this std::unique_lock.
 * 
 * The method lk.release() returns the mutex; therefore, you have to unlock it manually.
 *
 * Thanks to std::unique_lock, it is quite easy to lock many mutexes in one atomic step.
 * Therefore you can overcome deadlocks by locking mutexes in a different order. 
 */


#pragma once
#include <iostream>
#include <chrono>
#include <mutex>
#include <thread>

namespace unique_lock {

struct CriticalData {
    std::mutex m;
};

void dead_lock(CriticalData& a, CriticalData& b) {
    // solve deadlock by locking mutexes in a different order.
    a.m.lock();
    std::cout << "get the first mutex\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    b.m.lock();
    std::cout << "get the second mutex\n";
    // do something with a and b
    a.m.unlock();
    b.m.unlock();
}

void test_dead_lock() {
    CriticalData c1, c2;

    std::thread t1([&]() { dead_lock(c1, c2); });
    std::thread t2([&]() { dead_lock(c1, c2); });
    t1.join(); t2.join();
}

/**
 * If you call the constructor of std::unique_lock with std::defer_lock, the underlying
 * mutex is not locked automatically. At this point (lines 74 and 79), the std::unique_lock
 * is just the onwer of the mutex. Thanks to the variadic template std::lock, the lock
 * operation is performed in an atomic step (line 83)
 */
void dead_lock2(CriticalData& a, CriticalData& b) {
    // solve deadlock by using a std::unique_lock (delayed locking of mutexes)
    std::unique_lock guard1(a.m, std::defer_lock);
    std::cout << "Thread: " << std::this_thread::get_id() << " first mutex\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    std::unique_lock guard2(b.m, std::defer_lock);
    std::cout << "  Thread: " << std::this_thread::get_id() << " second mutex\n";

    std::cout << "    Thread: " << std::this_thread::get_id() << " get both mutex\n";
    std::lock(guard1, guard2);
    // do something with a and b
}

void test_dead_lock2() {
    CriticalData c1, c2;

    std::thread t1([&]() { dead_lock2(c1, c2); });
    std::thread t2([&]() { dead_lock2(c1, c2); });
    t1.join(); t2.join();
}

}