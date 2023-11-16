/**
 * With C++17, the resolution of the deadlock becomes quite easy. We have
 * the std::scoped_lock that can lock an arbitrary of mutexes automatically.
 * You only have to use a std::scoped_lock instead of t he std::lock call. 
 */

#pragma once
#include <iostream>
#include <mutex>
#include <thread>

namespace scoped_lock {

struct CriticalData {
    std::mutex m;
};

void dead_lock(CriticalData& a, CriticalData& b) {
    std::cout << "Thread: " << std::this_thread::get_id() << " first mutex\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    std::cout << "  Thread: " << std::this_thread::get_id() << " second mutex\n";
    std::cout << "    Thread: " << std::this_thread::get_id() << " get both mutex\n";

    std::scoped_lock(a.m, b.m);
    // do something with a and b
}

void test_dead_lock() {
    CriticalData c1, c2;

    std::thread t1([&]() { dead_lock(c1, c2); });
    std::thread t2([&]() { dead_lock(c1, c2); });
    t1.join(); t2.join();
}

}