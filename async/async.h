#pragma once
#include <iostream>
#include <future>
#include <chrono>
#include <thread>
#include <random>
#include <vector>
#include <numeric>
#include <deque>
#include <utility>

namespace async {

/**********************************************************************************
 * `std::async` is the easiest way to create a future.
 * 
 * The C++ runtime decides if `std::async` is executed in a separate thread or not.
 * The decision of the C++ runtime may depend on the number of CPU codes available,
 * the utilization of your system, or the size of your work package.
 * By using `std::async` you only specify the task that should run. The C++ runtime
 * automatically manages the creation and also the lifetime of the thread.
 */

void async_basic()
{
    /*
     * `std::async` behaves like an asynchronous function call. This function call takes
     * a callable together with its arguments. `std::async` is a variadic template and 
     * can, therefore, take an arbitrary number of arguments.
     * The call to `std::async` returns a future object `fut`.
     */
    std::cout << std::endl;

    auto begin = std::chrono::system_clock::now();

    // With the start policy you can explicitly specify whether the async call should
    // be executed in the same thread (`std::launch::deferred`) or in another thread (`std::launch::async`).
    // Check thread id in the code below.

    auto async_lazy = std::async(std::launch::deferred, 
                                []() { std::cout << "async_lazy id: " << std::this_thread::get_id() << std::endl; return std::chrono::system_clock::now(); });
    auto async_eager = std::async(std::launch::async,
                                []() { std::cout << "async_eager id: " << std::this_thread::get_id() << std::endl; return std::chrono::system_clock::now(); });
    
    std::this_thread::sleep_for(std::chrono::seconds(1));

    auto lazy_start = async_lazy.get() - begin;
    auto eager_start = async_eager.get() - begin;
    auto lazy_duration = std::chrono::duration<double>(lazy_start).count();
    auto eager_duration = std::chrono::duration<double>(eager_start).count();

    std::cout << "main id: " << std::this_thread::get_id() << std::endl;
    std::cout << "async_lazy evaluated after : " << lazy_duration << " seconds.\n";
    std::cout << "async_eager evaluated after : " << eager_duration << " seconds.\n";
    std::cout << "-------------------------------------------------------" << std::endl << std::endl;

    // The future can request the result of the task only once by calling `fut.get()`.
    // Calling it more than once results in undefined behavior.

    // The call `async_lazy.get()` in line 40 triggers the execution of the promise in line 33
    // - the result is available after a short nap of one second (line 38).
    // But for `async_eager`, call `get()` gets the result from the immediatly executed work packge.
}

void start_policy()
{
    struct X
    {
        void foo(int, std::string const&) {
            std::cout << "X::foo(int, std::string const&)\n";
        }
        std::string bar(std::string const&) {
            std::cout << "X::bar(std::string const&)\n";
            return std::string("bar");
        }
    };

    struct Y
    {
        double operator()(double) { 
            std::cout << "Y::operator()(double)\n";
            return 5.0;
        }
    };

    auto baz = [](X&) {
        X x;
        return x;
    };
    X x;
    Y y;

    std::cout << std::endl;

    // Run in new thread
    auto f1 = std::async(std::launch::async, Y(), 1.2);

    // Run in wait() or get()
    auto f2 = std::async(std::launch::deferred, baz, std::ref(x));

    // Implementation choose
    auto f3 = std::async(
        std::launch::deferred | std::launch::async,
        baz, std::ref(x)
    );
    auto f4 = std::async(baz, std::ref(x));

    // Invoke deferred function
    f2.wait();

    std::cout << "-------------------------------------------------------" << std::endl << std::endl;
    /**
     * By default, it's up to the implementation whether `std::async` starts a new  thread,
     * or whether the task runs synchronously when the future is waited for.
     * In most cases this is what you want, but you can specify which to use with an additional
     * parameter to `std::async` before the function to call. This parameter is of the type `std::launch`:
     * - `std::launch::deferred` to indicate that the function call is to be deffered
     *   until either `wait()` or `get()` is called on the future
     * - `std::launch::async` to indicate that the function must be run on its own thread
     * - `std::launch::deferred | std::launch:async` to indicate that the implementation may choose.
     *   This last option is the default.
     * 
     * If the function call is deferred, it may never run.
     */
}

void fire_and_forget()
{
    /**
     * Fire and forget futures are special futures. They execute just in place because their future is 
     * not bound to a variable. It is necessary for a fire and forget future that the promise runs in
     * a separate thread so it can immediately shart its work. This is doen by the `std::launch::async` policy.
     */
    std::cout << std::endl;
    auto begin = std::chrono::system_clock::now();

    std::async(std::launch::async, []() {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << "first thread\n";
    });
    std::cout << "first thread done: " << std::chrono::duration<double>(std::chrono::system_clock::now() - begin).count() << " seconds.\n";
    std::async(std::launch::async, []() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "second thread\n";
    });
    std::cout << "second thread done: " << std::chrono::duration<double>(std::chrono::system_clock::now() - begin).count() << " seconds.\n";
    std::cout << "main thread\n";
    std::cout << "-------------------------------------------------------" << std::endl << std::endl;

    // Fire and forget futures look very promising but have a big drawback.
    // A future that is created by `std::async` waits on its destructor, until its promis is doen.
    // In this context, waiting is not very different from blocking. The future blocks the progress
    // of the program in its destructor. This becomes more evident in this code.
    //
    // This code executes two promises in their threads.
    // The resulting futures are fire and forget futures. These futures block in their destructors until
    // the associated promise is done. The result is that the promise is executed in the sequence.
    // The execution sequence is indepedent of the execution time.
}

void concurrent_calculation()
{
    // The calculation of the scalar product that be spread across four asynchronous function calls.
    int const NUM = 1000000;

    auto get_dot_proudct = [](std::vector<int>& v, std::vector<int>& w) {
        auto size = v.size();

        auto future1 = std::async([&]() {
            return std::inner_product(&v[0], &v[size / 4], &w[0], 0LL);
        });
        auto future2 = std::async([&]() {
            return std::inner_product(&v[size / 4], &v[size / 2], &w[size / 4], 0LL);
        });
        auto future3 = std::async([&]() {
            return std::inner_product(&v[size / 2], &v[size * 3/4], &w[size / 2], 0LL);
        });
        auto future4 = std::async([&]() {
            return std::inner_product(&v[size * 3/4], &v[size], &w[size * 3/4], 0LL);
        });

        return future1.get() + future2.get() + future3.get() + future4.get();
    };

    std::cout << std::endl;
    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_int_distribution<int> dist(0, 100);

    // fill the vectors
    std::vector<int> v, w;
    v.reserve(NUM);
    w.reserve(NUM);
    long long result = 0LL;
    for (int i = 0; i < NUM; i++) {
        v.push_back(dist(engine));
        w.push_back(dist(engine));
        result += v.back() * w.back();
    }

    std::cout << "get_dot_product(v, w) : " << get_dot_proudct(v, w) << " (expected: " << result << ")"<< std::endl;
    std::cout << "-------------------------------------------------------" << std::endl << std::endl;
}

void passing_args_to_async()
{
    std::cout << std::endl;
    struct X
    {
        void foo(int, std::string const&) {
            std::cout << "X::foo(int, std::string const&)\n";
        }
        std::string bar(std::string const&) {
            std::cout << "X::bar(std::string const&)\n";
            return std::string("bar");
        }
    };
    X x;

    // Calls p->foo(42, "hello") where p is &x
    auto f1 = std::async(&X::foo, &x, 42, "hello");
    // Calls tmpx.bar("goodbye") where tmpx is a copy of x
    auto f2 = std::async(&X::bar, x, "goodbye");

    struct Y
    {
        double operator()(double) { 
            std::cout << "Y::operator()(double)\n";
            return 5.0;
        }
    };
    Y y;

    // Calls tmpy(3.141) where tmpy is move-constructed from Y()
    auto f3 = std::async(Y(), 3.141);
    // Calls y(2.718)
    auto f4 = std::async(std::ref(y), 2.718);

    auto baz = [](X&) {
        X x;
        return x;
    };
    // Calls baz(x)
    std::async(baz, std::ref(x));

    class move_only
    {
    public:
        move_only() {}
        move_only(move_only&&) {}
        move_only(move_only const&) = delete;
        move_only& operator=(move_only&&) {}
        move_only& operator=(move_only const&) = delete;
        void operator()() {}
    };
    // Calls tmp() where tmp is constructed from std::move(move_only())
    auto f5 = std::async(move_only());
    
    std::cout << "-------------------------------------------------------" << std::endl << std::endl;
}

void package_task()
{
    /**
     * `std::package_task` is usually to perform a concurrent computation like `concurrent_calculatoin()` example.
     * 
     * `std::package_task pack` is a warpper for a callable in order to be invoked asynchronously. By calling
     * `pack.get_future()` you get the associated future. Invoking the call operator on pack (`pack()`) executes
     * the `std::packaged_task` and, therefor, executes the callable.
     * 
     * `std::packaged_task` usually consists of 4 steps:
     * 1. Warp you work
     * 2. Create a future
     * 3. Perform the calculation
     * 4. Query the result
     */
    class SumUp {
    public:
        int operator()(int beg, int end) {
            long long int sum{};
            for (int i = beg; i < end; i++) sum += i;
            return sum;
        }
    };

    std::cout << std::endl;

    SumUp sum_up1;
    SumUp sum_up2;
    SumUp sum_up3;
    SumUp sum_up4;

    // wrap the tasks
    std::packaged_task<int(int, int)> sum_task1(sum_up1);
    std::packaged_task<int(int, int)> sum_task2(sum_up2);
    std::packaged_task<int(int, int)> sum_task3(sum_up3);
    std::packaged_task<int(int, int)> sum_task4(sum_up4);

    // create the futures
    auto sum_result1 = sum_task1.get_future();
    auto sum_result2 = sum_task2.get_future();
    auto sum_result3 = sum_task3.get_future();
    auto sum_result4 = sum_task4.get_future();

    // push the tasks on the container
    std::deque<std::packaged_task<int(int, int)>> all_tasks;
    all_tasks.push_back(std::move(sum_task1));
    all_tasks.push_back(std::move(sum_task2));
    all_tasks.push_back(std::move(sum_task3));
    all_tasks.push_back(std::move(sum_task4));

    int begin{1}, increment{2500};
    int end = begin + increment;


    auto start = std::chrono::system_clock::now();
    // perform each calculation in a separate thread
    while (not all_tasks.empty()) {
        // `std::packaged_task` objects are not copyable.
        std::packaged_task<int(int, int)> my_task = std::move(all_tasks.front());
        all_tasks.pop_front();

        std::thread sum_thread(std::move(my_task), begin, end);
        begin = end;
        end += increment;
        sum_thread.detach();
    }
    // for (auto& task : all_tasks) {
    //     task(begin, end);
    //     begin = end;
    //     end += increment;
    // }

    // pick up the results
    auto sum = sum_result1.get() + sum_result2.get() + sum_result3.get() + sum_result4.get();

    auto finish = std::chrono::system_clock::now();
    
    std::cout << std::fixed;
    std::cout << "sum of 0 .. 10000 = " << sum << " (elapsed time: " << std::chrono::duration<double>(finish - start).count() << " sec)" <<std::endl;
    std::cout << "-------------------------------------------------------" << std::endl << std::endl;
}

void promise_and_future()
{
    /**
     * The class templates `std::promise` and `std::future` provide you with the ful control over the task.
     * 
     * Promise and future are a mighty pair. A promise can put a value, an exception, or simply a notification
     * into the shared data channel. One promise can serve many `std::shared_future` futures. 
     */
    {
        std::cout << std::endl;

        auto product = [](std::promise<int>&& int_promise, int a, int b) {
            int_promise.set_value(a * b);
        };

        class Div {
        public:
            void operator()(std::promise<int>&& int_promise, int a, int b) const {
                int_promise.set_value(a / b);
            }
        };

        int a = 20;
        int b = 10;

        // define the promies
        std::promise<int> prod_promise;
        std::promise<int> div_promise;

        // get the futures
        std::future<int> prod_result = prod_promise.get_future();
        std::future<int> div_result = div_promise.get_future();

        // calculate the result in a separate thread
        std::thread prod_thread(product, std::move(prod_promise), a, b);
        Div div;
        std::thread div_thread(div, std::move(div_promise), a, b);

        // get the result
        std::cout << "20 * 10 = " << prod_result.get() << std::endl;
        std::cout << "20 / 10 = " << div_result.get() << std::endl;
        std::cout << "-------------------------------------------------------" << std::endl << std::endl;

        prod_thread.join(); div_thread.join();
    }

    /**
     * `std::promise` enables you to set a value, a notification, or an exception.
     * In addition the promise can provide its result in a delayed fashion.
     * 
     * `std::future` enables you to
     * - pick up the value from the promise.
     * - ask the promise if the value is available
     * - wait for the notification of the promise. This waiting can be done with a relative time duration or
     *   an absolute time point.
     * - create a shared future (`std::shared_future`)
     * 
     * `wait_for` and `wait_until` calls of the future and a shared future return its state.
     * Three states are possible:
     * - `future_status::ready`
     * - `future_status::timeout`
     * - `future_status::deferred`
     */
    {
        using namespace std::literals::chrono_literals;
        std::cout << std::endl;

        auto get_answer = [](std::promise<int> int_promise) {
            std::this_thread::sleep_for(3s);
            int_promise.set_value(42);
        };

        std::promise<int> answer_promise;
        auto fut = answer_promise.get_future();

        std::thread t(get_answer, std::move(answer_promise));

        std::future_status status{};
        do {
            status = fut.wait_for(0.2s);
            std::cout << "... doing something else\n";
        } while (status != std::future_status::ready);

        std::cout << std::endl
                << "The Answer: " << fut.get() << std::endl;
        std::cout << "-------------------------------------------------------" << std::endl << std::endl;
        t.join();
    }
}

}