#pragma once
#include <iostream>
#include <vector>
#include <chrono>
#include <numeric>
#include <mutex>
#include <thread>
#include <future>

namespace threadlocal {

void use_local_var(std::vector<int> const& v)
{
    long long size = v.size();
    std::cout << "------------ Using a Local Variable\n";
    std::mutex mutex;
    auto sum_up = [&mutex](long long& sum, std::vector<int> const& v, long long const beg, long long const end) {
        long long tmp_sum{};
        for (auto i = beg; i < end; i++) {
            tmp_sum += v[i];
        }
        std::lock_guard lock(mutex);
        sum += tmp_sum;
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
        long long tmp_sum{};
        for (auto i = beg; i < end; i++) {
            tmp_sum += v[i];
        }
        sum += tmp_sum;
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
        long long tmp_sum{};
        for (auto i = beg; i < end; i++) {
            tmp_sum += v[i];
        }
        sum.fetch_add(tmp_sum, std::memory_order_relaxed);
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

thread_local long long tmp_sum{};
void use_threadlocal_data(std::vector<int> const& v) {
    long long size = v.size();
    std::cout << "------------ Using a Thread-Local Data\n";
    auto sum_up = [](std::atomic<long long>& sum, std::vector<int> const& v, long long const beg, long long const end) {
        for (auto i = beg; i < end; i++) {
            tmp_sum += v[i];
        }
        sum.fetch_add(tmp_sum, std::memory_order_relaxed);
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

void use_task(std::vector<int> const& v) {
    long long size = v.size();
    std::cout << "------------ Using a Task\n";
    auto sum_up = [](std::promise<long long>&& promise, std::vector<int> const& v, long long const beg, long long const end) {
        long long sum{};
        for (auto i = beg; i < end; i++) {
            sum += v[i];
        }
        promise.set_value(sum);
    };

    std::promise<long long> promise1, promise2, promise3, promise4;
    auto future1 = promise1.get_future();
    auto future2 = promise2.get_future();
    auto future3 = promise3.get_future();
    auto future4 = promise4.get_future();

    auto start = std::chrono::steady_clock::now();
    
    std::thread t1(sum_up, std::move(promise1), std::ref(v), 0, size / 4);
    std::thread t2(sum_up, std::move(promise2), std::ref(v), size / 4, size / 2);
    std::thread t3(sum_up, std::move(promise3), std::ref(v), size / 2, size * 3/4);
    std::thread t4(sum_up, std::move(promise4), std::ref(v), size * 3/4, size);
    
    auto sum = future1.get() + future2.get() + future3.get() + future4.get();

    std::chrono::duration<double> duration = std::chrono::steady_clock::now() - start;
    std::cout << "Time for addition " << duration.count() << " seconds (result: " << sum << ")\n";

    t1.join(); t2.join(); t3.join(); t4.join();
}

}