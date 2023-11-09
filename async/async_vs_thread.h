#pragma once
#include <iostream>
#include <future>
#include <thread>

namespace async_vs_thread {

/**********************************************************************************
 * In addition to threads, C++ has tasks to perform work asynchronously. 
 * Tasks need the <future> header. A task is parameterized with a work package,
 * and consists of the two associated components: a promise and a future.
 * Both are connected via a data channel.
 * 
 * The promise executes the work packages and puts the result in the data channel
 * The associated future picks up the result. Both communication endpoints can run in
 * separate threads.
 * 
 * Tasks behave like data channels between communication endpoints (a promise and a future).
 * These endpoints can exist in the same or in different threads.
 */

void exam()
{
    std::cout << std::endl;

    int result;
    std::thread t([&]() {
        result = 2000 + 11;
    });
    t.join();
    std::cout << "result: " << result << std::endl;

    auto fut = std::async([]() { return 2000 + 11; });
    std::cout << "fut.get(): " << fut.get() << std::endl;
    std::cout << "-------------------------------------------------------" << std::endl << std::endl;

    // The future can request the result of the task only once by calling `fut.get()`.
    // Calling it more than once results in undefined behavior.
}

}