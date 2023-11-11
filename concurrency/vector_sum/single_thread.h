#pragma once
#include <iostream>
#include <vector>
#include <chrono>
#include <numeric>
#include <mutex>
#include <thread>

namespace single_thread {

void range_baseed_for_loop(std::vector<int> const& v) {
    std::cout << "------------ Range-based for Loop\n";
    long long sum{};

    auto start = std::chrono::steady_clock::now();
    for (auto n : v) sum += n;

    const std::chrono::duration<double> duration = std::chrono::steady_clock::now() - start;

    std::cout << "Time for addition " << duration.count() << " seconds (result: " << sum << ")\n";
}

void sum_by_accum(std::vector<int> const& v) {
    std::cout << "------------ Summation with std::accumulate\n";
    long long sum{};

    auto start = std::chrono::steady_clock::now();
    sum = std::accumulate(v.begin(), v.end(), 0LL);

    const std::chrono::duration<double> duration = std::chrono::steady_clock::now() - start;

    std::cout << "Time for addition " << duration.count() << " seconds (result: " << sum << ")\n";
}

void sum_with_a_lock(std::vector<int> const& v) {
    std::cout << "------------ Protection with a Lock\n";
    std::mutex mutex;
    long long sum{};

    auto start = std::chrono::steady_clock::now();
    for (auto n : v) {
        std::lock_guard lock(mutex);
        sum += n;
    }

    const std::chrono::duration<double> duration = std::chrono::steady_clock::now() - start;

    std::cout << "Time for addition " << duration.count() << " seconds (result: " << sum << ")\n";
}

void sum_with_atomics(std::vector<int> const& v) {
    std::cout << "------------ Protection with Atomics\n";
    std::mutex mutex;
    std::atomic<long long> sum{};
    std::cout << std::boolalpha << "sum.is_lock_free(): " << sum.is_lock_free() << std::endl << std::endl;

    auto start = std::chrono::steady_clock::now();
    for (auto n : v) {
        sum += n;
    }
    std::chrono::duration<double> duration = std::chrono::steady_clock::now() - start;
    std::cout << "Time for addition " << duration.count() << " seconds (result: " << sum << ")\n";

    sum = 0;
    start = std::chrono::steady_clock::now();
    for (auto n : v) {
        sum.fetch_add(n);
    }
    duration = std::chrono::steady_clock::now() - start;
    std::cout << "Time for addition " << duration.count() << " seconds (result: " << sum << ")\n";
}

}