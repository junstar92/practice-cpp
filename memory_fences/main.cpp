#include <iostream>
#include <atomic>
#include <thread>
#include <string>
#include <cassert>
#include <csignal>

// C++ support two kind of fences: 
// - `std::atomic_thread_fence`
// - `std::atomic_signal_fence`

void acquire_release_with_fences()
{
    std::cout << "--------- Example for std::atomic_thread_fence ---------" << std::endl;
    // acquire-release without fences
    {
        std::atomic<std::string*> ptr{nullptr};
        int data;
        std::atomic<int> atomic_data;

        auto producer = [&]() {
            std::string* p = new std::string("C++11");
            data = 2011;
            atomic_data.store(2014, std::memory_order_relaxed);
            ptr.store(p, std::memory_order_release);
        };

        auto consumer = [&]() {
            std::string* p2;
            while (!(p2 = ptr.load(std::memory_order_acquire)));
            std::cout << "*p2: " << *p2 << std::endl;
            std::cout << "data: " << data << std::endl;
            std::cout << "atomic_data: " << atomic_data.load(std::memory_order_relaxed) << std::endl;
        };

        std::thread t1(producer);
        std::thread t2{consumer};
        t1.join(); t2.join();

        delete ptr;
        std::cout << std::endl;
        
        // 1. Line 19 - 21 happens-before line line 22: `ptr.store(p, std::memory_order_release)`.
        // 2. Line 27: `while (!(p2 = ptr.load(std::memory_order_acquire)))` happens-before the lines 28 - 30.
        // 3. Line 22 synchronizes-with line 27 => Line 22 inter-thread happens-before line 27.
    }
    // acquire-release with fences
    {
        std::atomic<std::string*> ptr{nullptr};
        int data;
        std::atomic<int> atomic_data;

        auto producer = [&]() {
            std::string* p = new std::string("C++11");
            data = 2011;
            atomic_data.store(2014, std::memory_order_relaxed);
            std::atomic_thread_fence(std::memory_order_release);
            ptr.store(p, std::memory_order_relaxed);
        };

        auto consumer = [&]() {
            std::string* p2;
            while (!(p2 = ptr.load(std::memory_order_relaxed)));
            std::atomic_thread_fence(std::memory_order_acquire);
            std::cout << "*p2: " << *p2 << std::endl;
            std::cout << "data: " << data << std::endl;
            std::cout << "atomic_data: " << atomic_data.load(std::memory_order_relaxed);
        };

        std::thread t1(producer);
        std::thread t2{consumer};
        t1.join(); t2.join();

        delete ptr;
        std::cout << std::endl;

        // 1. The acquire and release fences prevent the reordering of the atomic
        //    and non-atomic operations across the fences.
        // 2. The consumer thread t2 is waiting in the while loop,
        //    until the pointer `ptr.store(p, std::memory_order_relaxed)` is set in the producer thread t1
        // 3. The release fence synchronizes-with the acquire fence.
        // 4. In the end, all effects of relaxed or non-atomic operations that happens-before
        //    the release fence via visible after the acquire fence.
    }
    std::cout << "-------------------------------------------------------" << std::endl << std::endl;
}

std::atomic<bool> a{false};
std::atomic<bool> b{false};
extern "C"
void handler(int) {
    if (a.load(std::memory_order_relaxed)) {
        std::atomic_signal_fence(std::memory_order_acquire);
        assert(b.load(std::memory_order_relaxed));
    }
}

void atomic_signal_fence()
{
    // std::atomic_signal_fence establishes memory synchronization ordering of 
    // non-atomic and relzed atomic accesses between a thread and a signal handler executed on the same thread.
    std::cout << "--------- Example for std::atomic_signal_fence --------" << std::endl;

    {
        std::signal(SIGTERM, handler);

        b.store(true, std::memory_order_relaxed);
        std::atomic_signal_fence(std::memory_order_release);
        a.store(true, std::memory_order_relaxed);

        // std::atomic_signal_fence(std::memory_order_release) (line 108) is the release operation
        // std::atomic_signal_fence(std::memory_order_acquire) (line 93) is the acquire operation
        // This means in particular that release operations can not be reordered across the release fence (line 108)
        // and the acquire operations can not be reordered across the acquire fence (line 93).
        // Consequently, the assertion in line 94 never fires because if `a.store(true, std::memory_order_relaxed)`
        // happened, `b.store(true, std::memory_order_relaxed)` must have happened before.
    }

    std::cout << "-------------------------------------------------------" << std::endl << std::endl;
}

int main()
{
    acquire_release_with_fences();
    atomic_signal_fence();
}