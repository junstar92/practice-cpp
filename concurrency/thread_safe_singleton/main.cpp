#include <iostream>
#include <chrono>
#include <future>

#include "simple_locking.h"
#include "double_checked_locking.h"
#include "meyers_singleton.h"
#include "lock_guard.h"
#include "call_once.h"
#include "atomic.h"

constexpr size_t ten = 10000000;
constexpr size_t fourty = 4 * ten;

template<typename Singleton>
void single_thread_test()
{
    const auto begin = std::chrono::system_clock::now();

    for (size_t i = 0; i < fourty; i++) {
        Singleton::get_instance();
    }

    const auto end = std::chrono::system_clock::now();

    std::cout << "> elapsed time : " << std::chrono::duration<double>(end - begin).count() << " sec\n";
}

template<typename Singleton>
void multi_thread_test()
{
    auto get_time = []() {
        const auto begin = std::chrono::system_clock::now();

        for (size_t i = 0; i < ten; i++) {
            Singleton::get_instance();
        }

        return std::chrono::system_clock::now() - begin;
    };

    auto future1 = std::async(std::launch::async, get_time);
    auto future2 = std::async(std::launch::async, get_time);
    auto future3 = std::async(std::launch::async, get_time);
    auto future4 = std::async(std::launch::async, get_time);

    const auto total = future1.get() + future2.get() + future3.get() + future4.get();

    std::cout << "> elapsed time : " << std::chrono::duration<double>(total).count() << " sec\n";
}

int main()
{
    /** Simple Thread-Safe Singleton Implementation
     * It is the classical way to initialize a singleton in a thread-safe. The first idea to 
     * implement a thread-safe singleton is to protect the initialization of the singleton with a lock.
     * 
     * Are there any issues ?
     * - Yes, because there is a considerable performance penalty.
     * - Also, No, because the implementation is thread-safe.
     * 
     * A heavyweight lock protects each access to the singleton in simple_locking.h:13. This is
     * also applied to the read access, which after the initial construction of `Singleton` is not necessary.
     */
    std::cout << "\n======== Simple Thread-Safe Singleton (single thread test)\n";
    single_thread_test<simple_lock::Singleton>();

    /** Double-Checked Locking Pattern
     * `double_lock::Singleton` removes unnecessary lock. Instead of the heavyweight lock, It uses a
     * lightweight pointer comparison in double_checked_locking.h:11. If it gets a null pointer, applies
     * the heavyweight lock on the singleton. Because there is the possibility that another thread
     * initializes the singleton between the pointer comparison and the lock call, I have to perform an
     * additional pointer comparison in double_checked_locking.h:13. It is why the name is called
     * double-checked locking pattern - two times a check and one time a lock.
     * 
     * It is smart, but not thread-safe.
     * The call `instance = new Singleton()` in double_chekced_locking.h:13 consists of at least three steps.
     * 1. Allocate memory for `Singleton`
     * 2. Initializes the `Singleton` object
     * 3. Let instance refer to the fully initialized `Singleton` object
     * 
     * The issue is that the C++ runtime provides no guarantee that the steps are performed in that sequence.
     * For example, it is possible that the processor may reorder the stpes to the sequence 1, 3 and 2. So
     * in the first step, the memory is allocated, and in the second step `instance` refers to a non-initialized
     * singleton. If just at the moment another thread t2 tries to access the singleton and makes the pointer
     * comparison, the comparison succeeds. The consequence is that thread t2 refers to a non-intialized singleton,
     * and the program behavior is undefined.
     */
    std::cout << "\n======== Double-Checked Locking Pattern (single thread test)\n";
    single_thread_test<double_lock::Singleton>();

    /** Thread-Safe Meyers Singleton
     * The C++11 standard guarantees that static variables with block scope are initialized in a thread-safe way.
     * The Meyers Singleton uses a static variable with block scope, so we are done.
     * 
     * - Static Variables with Block Scope
     * Static variables with block scope are created exactly once and lazily. Lazily means that they are created
     * just at the moment of the usage. This characteristic is the basis of the so-called Mayers Singleton. This is
     * by far the most elegant implementation of the singleton pattern in C++. With C++11, static variables with
     * block scope have an additional guarantee: they are initialized in a thread-safe way.
     * 
     * !! If you use the Meyers Singleton in a concurrent environment, be sure that your compiler implements static
     * variables with the C++11 thread-safe semantic. It happens quite often that programmers rely on the C++11 
     * semantic of static variables, but their compiler does not support it. The result may be that more than one
     * instance of a singleton is created.
     * 
     * The test function (multi_thread_test) is executed by the 4 promises. The results of the associated futures are summed
     * up.
     */
    std::cout << "\n======== Thread-Safe Meyers Singleton (multi thread test)\n";
    multi_thread_test<meyers_singleton::Singleton>();

    /** Singleton Using the Mutex with std::lock_guard
     * We can guess that this approach is pretty slow. 
     */
    std::cout << "\n======== Singleton Using the Mutex (multi thread test)\n";
    multi_thread_test<lock_guard::Singleton>();

    /** Singleton Using std::call_once with std::once_flag
     * You can use the function std::call_once together with the std::once_flag to register callables so that
     * exactly one callable is executed in a thread-safe way. It is faster than previous's.
     */
    std::cout << "\n======== Singleton Using the std::call_once (multi thread test)\n";
    multi_thread_test<call_once::Singleton>();

    /** Singleton Using the Atomics
     * With atomic variables, first implementation becomes a lot more challenging. It can even specify the
     * memory-ordering for the atomic operations. The following two implementations of the thread-safe
     * singletons are based on the previsouly mentioned double-checked locking pattern.
     */
    std::cout << "\n======== Singleton Using the Atomics with Sequential Consistency (multi thread test)\n";
    /**
     * In contrast to the double-checked locking pattern, it has the guarantee that the expression
     * `sin = new Singleton()` in atomic.h:12 happens before the store expression `instance.store(sin)` in atomic.h:18.
     * This is due to the sequential consistency as default-memory ordering for atomic operations. Have a look at atomic.h:15:
     * `sin = instance.load(std::memory_order_relaxed)`. This additional load is necessary because, between the first load (atomic.h:12)
     * and the usage of the lock (atomic.h:14), another thread may kick in and change the value of `instance`.
     * 
     */
    multi_thread_test<atomic::seq_cst::Singleton>();

    std::cout << "\n======== Singleton Using the Atomics with Acqure-Release Semantic (multi thread test)\n";
    /**
     * We can optimize the program even more.
     * Let's have a closer look at the previous thread-safe implementation of the singleton pattern using atomics.
     * The loading (or reading) of the singleton is an acquire operation, the storeing (or writing) is a release operation.
     * Both operations take place on the same atomic. Therefore sequential consistency is overkill. The C++11 standard
     * guarantees that a release operation synchronizes with an acquire operation on the same atomic and estabilishes
     * an ordering constraint. This means that all previous read and write operations cannot be moved after a release operation,
     * and all subsequent read and write operations cannot be moved before an acquire operation.
     * 
     * These are the minimum quarantees required to implement a thread-safe singleton.
     * 
     * The acquire-release semantic has similar performance as the sequential consistency. It is because on the x86
     * architecture both memory-orderings are very similar (It would probably more significant difference in the
     * performance numbers on the ARM or PowerPC architecture).
     */
    multi_thread_test<atomic::acq_rel::Singleton>();
}