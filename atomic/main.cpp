#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <chrono>

#include "spin_lock.h"

void sequence_consistency()
{
    std::cout << "-------------- Weak Memory Model Example --------------" << std::endl;
    // Example for Weak Memory Model (atomic default memory model: memory_order_seq_cst)
    //
    // sequence consistency guarantees
    // 1. The instruction of a program are executed in the order written down.
    // 2. There is a global order of all operations on all threads.

    std::atomic<int> x{256}, y{256};

    int res1{}, res2{};

    std::thread t1([&]() {
        x.store(1); // is equivalent to `x.store(1, std::memory_order_seq_cst)`
        res1 = y.load(); // is equivalent to `x.load(std::memory_order_seq_cst)`
    });
    std::thread t2([&]() {
        y.store(1); // is equivalent to `y.store(1, std::memory_order_seq_cst)`
        res2 = x.load(); // is equivalent to `y.load(std::memory_order_seq_cst)`
    });

    t1.join();
    t2.join();

    // possible results are
    // 1. res1 = 256, re2 = 1
    // 2. res1 = 1, res2 = 1
    // 3. res1 = 1, res2 = 256
    std::cout << "res1 = " << res1 << " / res2 = " << res2 << std::endl;
    std::cout << "-------------------------------------------------------" << std::endl << std::endl;
}

void atomic_flag()
{
    std::cout << "------------ Atomic Flag (SpinLock) Example -----------" << std::endl;
    // Example for Atomic Flag by Using SpinLock
    //
    // std::atomic_falg
    // 1. the only lock-free atomic.
    // 2. the building block for higher-level thread abstraction.
    //
    // SpinLock is an elementary lock such as a mutex.
    // It contrast to mutex, it waits not until it gets its lock.
    // SpinLock saves the expensive context switch in the wait state from the user space to kernel space,
    // but it entirely utilises the CPU and wastes CPU cycles.
    
    SpinLock spin;
    std::atomic<int> x{256}, y{256};

    int res1 = 0, res2 = 0;

    std::thread t1([&]() {
        spin.lock();
        // critical section
        x.store(1);
        res1 = y.load();
        spin.unlock();
    });
    std::thread t2([&]() {
        spin.lock();
        // critical section
        y.store(1);
        res2 = x.load();
        spin.unlock();
    });

    t1.join();
    t2.join();

    // possible results are
    // 1. res1 = 256, re2 = 1
    // 2. res1 = 1, res2 = 256
    std::cout << "res1 = " << res1 << " / res2 = " << res2 << std::endl;
    std::cout << "-------------------------------------------------------" << std::endl << std::endl;
}

void atomic_bool()
{
    std::cout << "-------------- std::atomic<bool> Example --------------" << std::endl;
    // Example for condition variable by using std::atomic<bool>

    std::vector<int> my_shared_work;
    std::atomic<bool> data_ready{false};

    auto waiting_for_work = [&]() {
        std::cout << "Waitinig" << std::endl;
        while (!data_ready.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        my_shared_work[1] = 2;
        std::cout << "Work done" << std::endl;
    };

    auto set_data_ready = [&]() {
        my_shared_work = {1, 0, 3};
        data_ready = true;
        std::cout << "Data prepared" << std::endl;
    };

    std::thread t1(waiting_for_work);
    std::thread t2(set_data_ready);

    t1.join();
    t2.join();

    for (auto v : my_shared_work) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
    
    std::cout << "-------------------------------------------------------" << std::endl << std::endl;
}

void atomic_with_pointer()
{
    std::cout << "--------------- std::atomic<T*> Example ---------------" << std::endl;
    // Example of std::atomic<T*>
    int arr[5];
    std::atomic<int*> p(arr);
    p++;
    assert(p.load() == &arr[1]);
    p += 1;
    assert(p.load() == &arr[2]);
    --p;
    assert(p.load() == &arr[1]);
    
    std::cout << "-------------------------------------------------------" << std::endl << std::endl;
}

template<typename T>
T fetch_mult(std::atomic<T>& shared, T mult) {
    T old_value = shared.load();
    while (!shared.compare_exchange_strong(old_value, old_value * mult));
    return old_value;
};

void atomic_with_arithmetic_type()
{
    std::cout << "--------- std::atomic<arithmetic type> Example --------" << std::endl;
    // Example of std::atomic<arithmetic  type>
    std::atomic<int> my_int{5};
    std::cout << "my_int: " << my_int << std::endl;
    fetch_mult(my_int, 5);
    std::cout << "my_int: " << my_int << std::endl;
    
    std::cout << "-------------------------------------------------------" << std::endl << std::endl;
}

int main()
{
    sequence_consistency();
    atomic_flag();
    atomic_bool();
    atomic_with_pointer();
    atomic_with_arithmetic_type();

    return 0;
}