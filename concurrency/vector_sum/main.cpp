#include <iostream>
#include <vector>
#include <random>

#include "single_thread.h"
#include "multi_therad.h"
#include "thread_local.h"

constexpr long long size = 100000000;

int main()
{
    std::vector<int> v;
    v.reserve(size);

    // random init
    std::random_device device;
    std::mt19937 engine(device());
    std::uniform_int_distribution<> dist(1, 10);
    for (long long i = 0; i < size; i++) v.push_back(dist(engine));

    std::cout << "\n***** single thread *****\n";
    /**
     * The straighforward strategy is to add the numbers in a range-based for loop.
     * You can use an algorithm from the STL (std::accumulate). In my case,
     * using `std::accumulate` makes the performance much better than range-based loop.
     * 
     * range-based loop: 0.039335 sec
     * std::accumulate : 0.017377 sec
     */
    single_thread::range_baseed_for_loop(v);
    single_thread::sum_by_accum(v);
    /**
     * If protect access to the summation variable with a lock, 
     * - Hwo expesive is the synchronization of a lock without contention ?
     * - How fast can a lock be in the optimal case ?
     * 
     * The access to the protected variable without contention is slower.
     * So, using a std::lock_guard is about more 100 times slower than using `std::accumulate`.
     * 
     * sum with a lock-guard: 1.44773 sec
     */
    single_thread::sum_with_a_lock(v);
    /**
     * Same questions for atomics.
     * - How expensive is the synchronization of an atomic ?
     * - How fast can an atomic be if there is no contention ?
     * 
     * Atomics are lock-free. And the summation is calculated in two ways: += operator and fetch_add method.
     * In the single threaded case both variatns have comparable performance.
     * 
     * += operator: 1.15304 sec
     * fetch_add  : 1.15279 sec
     */
    single_thread::sum_with_atomics(v);

    /**
     * Three points to emphasize in single threaded cases:
     * 1. Atomics are 12 - 50 times slower on Linux and Windows than std::accumulate
     *    without synchronization.
     * 2. Atomics are 2 - 3 times faster on Linux and Windows than locks.
     * 3. std::accumulate seems to be highly optimized on Windows.
     */



    /**
     * Using a shared variable for the summation with 4 threads is not optimal
     * because the synchronization overhead outweights the performance benefit.
     * 
     * The questions in multi-thread cases are below:
     * - What is the difference in performance between the summation using a lock an an atomic ?
     * - What is the difference in performance between single threaded and multithreaded execution
     *   of `std::accumulate' ?
     */
    std::cout << "\n***** multi thread *****\n";
    /**
     * The simplest way to make the thread-safe summation is to use a `std::lock_guard`.
     * The `std::lock_guard` is used to protect the shared sum. Each thread performs
     * a quater of the summation.
     * 
     * The bottleneck of the program is the shared variable because a `std::lock_guard`
     * heavily synchronize it.
     * use_lock_guard(): 3.1861 sec
     */
    multithread_shared_var::use_lock_guard(v);
    /**
     * One obvious solution from `std::lock_guard` is to replace the heavyweight lock
     * with a lightweight atomic. Now, the summation variable is an atomic.
     * 
     * There is no performance difference between `std::lock_guard` and atomic.
     * The performance of using `fetch_add()` method is same.
     * 
     * use_atomic()   : 3.33947 sec
     * use_fetch_add(): 3.33467 sec
     */
    multithread_shared_var::use_atomic(v);
    multithread_shared_var::use_fetch_add(v);
    /**
     * Although there is no performance difference between the += operator and the
     * `fetch_add` method on an atomic, `fetch_add` has an advantage: it allows to weaken
     * the memory-ordering explicitly and to apply relaxed semantic.
     * 
     * The default policy for atomics is sequential consistency.
     * The relaxed semantic is fine in this use-case because we have two guarantees:
     * each addition with `fetch_add` takes place atomically, and the threads synchronize
     * with join calls. Because of the weakest memory model, we have the best performance.
     * 
     * use_fetch_add_with_relaxed_semantic(): 1.28434 sec
     */
    multithread_shared_var::use_fetch_add_with_relaxed_semantic(v);
    /**
     * Using a shared atomic variable with relaxed semantic and calculating the sum with
     * the help of four threads is about 100 times slower than using a single-thread with 
     * algorithm `std::accumulate`.
     * 
     * Let's combine the two previous for adding the numbers: We use four threads and 
     * minimize the synchronization between the threads below.
     */

    /**
     * In Thread-Local Summation case, there are different ways to minimize the 
     * synchronization. We use local variables, thread-local data, and tasks.
     */
    std::cout << "\n***** thread-local *****\n";
    /**
     * Since each thread can use a local summation variable, it can do its job without
     * synchornization. The synchronization is only neccesary to sum up the local variables.
     * The summation of the local variables is the critical section that must be protected and
     * this can be done in various ways: 
     * - `std:lock_guard`                   : 0.007659 sec
     * - use an atomic                      : 0.007745 sec
     * - use an atomic with relaxed semantic: 0.008046 sec
     * 
     * There is no performance difference.
     */
    threadlocal::use_local_var(v);
    threadlocal::use_atomic(v);
    threadlocal::use_fetch_add_with_relaxed_semantic(v);
    /**
     * Thread-local data belongs to the thread in which in was created; it is only be created
     * when needed. Thread-local data is an ideal fit for the local summation variable `tmp_sum`.
     * 
     * use_threadlocal_data(): 0.008239 sec
     */
    threadlocal::use_threadlocal_data(v);
    /**
     * Using tasks, we can do the work whole job without explicit synchronization. Each partial
     * summation is performed in a separate thread, and the final summation takes place in the main
     * thread.
     * 
     * It does not make a big difference whether we use local variable or tasks for the calculation of
     * the partial sum or if we use various synchronization primitives such as an atomics.
     * 
     * use_task(): 0.009909 sec
     */
    threadlocal::use_task(v);
    /**
     * Conclusion:
     * 
     * The usage of a shared variable for the summation variable makes one point clear: 
     * synchronization is very expensive and should be avoided as much as possible.
     * Although we used an atomic variable and even broke the sequential consistency,
     * the 4 threads are 100 times slower than on thread. From a performance perspective,
     * minimizing expensive synchronization has to be the first goal.
     * 
     * The thread-local summation is only about two times faster than single-threaded method.
     * That holds even though each of the 4 threads can work independently. It is surprised because 
     * we can expect a nearly fourfold improvement. This reason is simple: the cores cannot get
     * the data fast enough from memory. The execution is `memory bound`. That is, memory slows
     * down the cores.
     * 
     * The Roofline model is an intuitive performance model to provide performance estimates of
     * applications running on multi-core, or many-core architecture. The model depends on the
     * peak performance, peak bandwidth, and arithmetic intensity of the architecture.
     */
}