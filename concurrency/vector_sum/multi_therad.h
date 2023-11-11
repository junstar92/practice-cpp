#pragma once
#include <iostream>
#include <vector>
#include <chrono>
#include <numeric>
#include <mutex>
#include <thread>

namespace multithread_shared_var {

void use_lock_guard(std::vector<int> const& v)
{
    long long size = v.size();
    std::cout << "------------ Using a std::lock_guard\n";
    std::mutex mutex;
    auto sum_up = [&mutex](long long& sum, std::vector<int> const& v, long long const beg, long long const end) {
        for (auto i = beg; i < end; i++) {
            std::lock_guard lock(mutex);
            sum += v[i];
        }
    };

    long long sum{};
    auto start = std::chrono::steady_clock::now();
    
    std::thread t1(sum_up, std::ref(sum), std::ref(v), 0, size / 4);
    std::thread t2(sum_up, std::ref(sum), std::ref(v), size / 4, size / 2);
    std::thread t3(sum_up, std::ref(sum), std::ref(v), size / 2, size * 3/4);
    std::thread t4(sum_up, std::ref(sum), std::ref(v), size * 3/4, size);
    t1.join(); t2.join(); t3.join(); t4.join();

    std::chrono::duration<double> duration = std::chrono::steady_clock::now() - start;
    std::cout << "Time for addition " << duration.count() << " seconds (result: " << sum << ")\n";
}

void use_atomic(std::vector<int> const& v) {
    long long size = v.size();
    std::cout << "------------ Using a Atomic Variable\n";
    auto sum_up = [](std::atomic<long long>& sum, std::vector<int> const& v, long long const beg, long long const end) {
        for (auto i = beg; i < end; i++) {
            sum += v[i];
        }
    };

    std::atomic<long long> sum{};
    auto start = std::chrono::steady_clock::now();
    
    std::thread t1(sum_up, std::ref(sum), std::ref(v), 0, size / 4);
    std::thread t2(sum_up, std::ref(sum), std::ref(v), size / 4, size / 2);
    std::thread t3(sum_up, std::ref(sum), std::ref(v), size / 2, size * 3/4);
    std::thread t4(sum_up, std::ref(sum), std::ref(v), size * 3/4, size);
    t1.join(); t2.join(); t3.join(); t4.join();

    std::chrono::duration<double> duration = std::chrono::steady_clock::now() - start;
    std::cout << "Time for addition " << duration.count() << " seconds (result: " << sum << ")\n";
}

void use_fetch_add(std::vector<int> const& v) {
    long long size = v.size();
    std::cout << "------------ Using a fetch_add()\n";
    auto sum_up = [](std::atomic<long long>& sum, std::vector<int> const& v, long long const beg, long long const end) {
        for (auto i = beg; i < end; i++) {
            sum.fetch_add(v[i]);
        }
    };

    std::atomic<long long> sum{};
    auto start = std::chrono::steady_clock::now();
    
    std::thread t1(sum_up, std::ref(sum), std::ref(v), 0, size / 4);
    std::thread t2(sum_up, std::ref(sum), std::ref(v), size / 4, size / 2);
    std::thread t3(sum_up, std::ref(sum), std::ref(v), size / 2, size * 3/4);
    std::thread t4(sum_up, std::ref(sum), std::ref(v), size * 3/4, size);
    t1.join(); t2.join(); t3.join(); t4.join();

    std::chrono::duration<double> duration = std::chrono::steady_clock::now() - start;
    std::cout << "Time for addition " << duration.count() << " seconds (result: " << sum << ")\n";
}

void use_fetch_add_with_relaxed_semantic(std::vector<int> const& v) {
    long long size = v.size();
    std::cout << "------------ Using a fetch_add() with relaxed semantic\n";
    auto sum_up = [](std::atomic<long long>& sum, std::vector<int> const& v, long long const beg, long long const end) {
        for (auto i = beg; i < end; i++) {
            sum.fetch_add(v[i], std::memory_order_relaxed);
        }
    };

    std::atomic<long long> sum{};
    auto start = std::chrono::steady_clock::now();
    
    std::thread t1(sum_up, std::ref(sum), std::ref(v), 0, size / 4);
    std::thread t2(sum_up, std::ref(sum), std::ref(v), size / 4, size / 2);
    std::thread t3(sum_up, std::ref(sum), std::ref(v), size / 2, size * 3/4);
    std::thread t4(sum_up, std::ref(sum), std::ref(v), size * 3/4, size);
    t1.join(); t2.join(); t3.join(); t4.join();

    std::chrono::duration<double> duration = std::chrono::steady_clock::now() - start;
    std::cout << "Time for addition " << duration.count() << " seconds (result: " << sum << ")\n";
}

}