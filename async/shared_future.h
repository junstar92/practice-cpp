#pragma once
#include <iostream>
#include <future>
#include <thread>

namespace shared_future {

/**
 * There are two ways to create a `std::shared_future`.
 * 
 * 1. You can take the future from the promise by a `get_future()`.
 * 2. You can either invoke `fut.shared()` on a future `fut`
 *    After the `fut.share()` call `fut.valid()` returns false.
 * 
 * A shared future is associated with its promise and can independetly ask for the result.
 * A `std::shared_future` has the same interface as a `std::future`.
 * 
 * The handling of a `std::shared_future` is special.
 * The following code creats directly a `std::shared_future`
 */

std::mutex cout_mutex;

struct Div {
    void operator()(std::promise<int>&& int_promise, int a, int b) {
        int_promise.set_value(a / b);
    }
};

struct Requestor {
    void operator()(std::shared_future<int> shared_fut) {
        // lock std::cout
        std::lock_guard<std::mutex> lock(cout_mutex);

        // get the thread id
        std::cout << "threadId(" << std::this_thread::get_id() << "): ";
        std::cout << "20 / 10 = " << shared_fut.get() << std::endl;
    }
};

void shared_future()
{
    std::cout << std::endl;

    // define the promise
    std::promise<int> div_promise;

    // get the future
    std::shared_future<int> div_result = div_promise.get_future();

    // calculate the result in a separate thread
    Div div;
    std::thread div_thread(div, std::move(div_promise), 20, 10);

    Requestor req;
    std::thread shared_thread1(req, div_result);
    std::thread shared_thread2(req, div_result);
    std::thread shared_thread3(req, div_result);
    std::thread shared_thread4(req, div_result);
    std::thread shared_thread5(req, div_result);

    div_thread.join();
    shared_thread1.join();
    shared_thread2.join();
    shared_thread3.join();
    shared_thread4.join();
    shared_thread5.join();
    
    std::cout << "-------------------------------------------------------" << std::endl << std::endl;
    /**
     * In line 53, `div_promise` is moved and executed in thread `div_thread`.
     * Accordingly, `std::shared_future`'s are copied in all five threads (line 56 - 60).
     * It is important to emphasise it once more. In contrast to a `std::future` object that
     * can only be moved, you can copy a `std::shared_future` object.
     */
}

void shared_future_from_future()
{
    std::cout << std::boolalpha << std::endl;

    // define the promise
    std::promise<int> div_promise;

    // get the future
    std::future<int> div_result = div_promise.get_future();
    std::cout << "div_result.valid(): " << div_result.valid() << std::endl;

    // calculate the result in a separate thread
    Div div;
    std::thread div_thread(div, std::move(div_promise), 20, 10);
    std::cout << "div_result.valid(): " << div_result.valid() << std::endl;

    std::shared_future<int> shared_result = div_result.share();
    std::cout << "div_result.valid(): " << div_result.valid() << std::endl;

    Requestor req;
    std::thread shared_thread1(req, shared_result);
    std::thread shared_thread2(req, shared_result);
    std::thread shared_thread3(req, shared_result);
    std::thread shared_thread4(req, shared_result);
    std::thread shared_thread5(req, shared_result);

    div_thread.join();
    shared_thread1.join();
    shared_thread2.join();
    shared_thread3.join();
    shared_thread4.join();
    shared_thread5.join();
    
    std::cout << "-------------------------------------------------------" << std::endl << std::endl;
    /**
     * The first two calls of `div_result.valid()` on the `std::future` (lines 87 and 92) return `true`.
     * This change after the call `div_result.share()` in line 94 because this call transfers the shared state.     * 
     */
}

}