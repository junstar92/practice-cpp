#pragma once
#include <iostream>
#include <future>
#include <utility>

namespace notification {

/**
 * Tasks are a save replacement for condition variables. If you use
 * promises and futures to synchronize threads, they have a lot in
 * common with condition variables.
 * 
 * The advantage of a condition variable is that you can use condition
 * variables to synchronize threads multiple times. In contrast to that,
 * a promise can send its notification only once, so you have to use
 * more promise and future pairs to get the functionality of a condition
 * variable. If you use the condition variable for only one synchronization,
 * the condition variable is a lot more challenging to use in the right way.
 * A promise and future pair needs no shared variable and therefore no lock,
 * and is not prone to spurious or lost wakeups. In addition to that, tasks
 * can handle exceptions.
 */

void do_the_work() {
    std::cout << "Processing shared data.\n";
}

void waiting_for_work(std::future<void>&& fut) {
    std::cout << "Worker: Waiting for work.\n";
    fut.wait();
    do_the_work();
    std::cout << "Work doen.\n";
}

void set_data_ready(std::promise<void>&& prom) {
    std::cout << "Sender: Data is ready.\n";
    prom.set_value();
}

void notification() {
    std::cout << std::endl;

    std::promise<void> send_ready;
    auto fut = send_ready.get_future();

    std::thread t1(waiting_for_work, std::move(fut));
    std::thread t2(set_data_ready, std::move(send_ready));

    t1.join();
    t2.join();

    std::cout << "-------------------------------------------------------" << std::endl << std::endl;
    /**
     * The function `set_data_ready` performs the notification part to the synchronization
     * - the function `waiting_for_work` the waiting part of the synchronization.
     */
}

}