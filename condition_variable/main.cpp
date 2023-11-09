#include <iostream>
#include <condition_variable>
#include <mutex>
#include <thread>

/*
 * - `condition_variable`'s methods
 * | Method                          |  Description
 * -------------------------------------------------------------------------------------
 * | notify_one()                    | Notifies a waiting thread.
 * | nofity_all()                    | Notifies all waiting threads.
 * | wait(lock, ...)                 | Waits for the notification while holding a std::unique_lock.
 * | wait_for(lock, rel_time, ...)   | Waits for a time duration for the notification while holding a std::unique_lock.
 * | wait_until(lock, abs_time, ...) | Waits until a time point for the notification while holding a std::unique_lock.
 * | native_handle()                 | Returns the native handle of this condition variable.
 */


void cond_var_exam()
{
    {
        std::mutex mutex;
        std::condition_variable cond_var;
        bool data_ready{false};

        auto do_the_work = [&]() {
            std::cout << "Processing shared data\n";
        };
        auto waiting_for_work = [&]() {
            std::cout << "Worker: Watining for work.\n";
            std::unique_lock lock(mutex);
            cond_var.wait(lock, [&data_ready]() { return data_ready; });
            do_the_work();
            std::cout << "Work done.\n";
        };
        auto set_data_ready = [&]() {
            {
                std::lock_guard lock(mutex);
                data_ready = true;
                std::cout << "Sender: Data is ready.\n";
            }
            cond_var.notify_one();
        };

        std::cout << std::endl;

        std::thread t1(waiting_for_work);
        std::thread t2(set_data_ready);

        t1.join(); t2.join();

        // This code has two child threads: `t1` and `t2`.
        // They get their work package `waiting_for_work` and `set_data_ready`.
        // `set_data_ready` (using `cond_var`) notifies that it is doen with the preparation of the work: `cond_var.notify_one()`.
        // While hodling the lock, thread `t1` waits for its notification: `cond_var.wait(lock, [](){ return data_ready; })`.

        // `cond_var.wait(lock, [](){ return data_ready; })` is equivalent to the following code:
        //
        // while (![](){ return data_ready; }()) {
        //   cond_var.wait(lock);
        // }
        //
        // `std::condition_variable::wait` is an optimization over a busy-wait.
        
        std::cout << "-------------------------------------------------------" << std::endl << std::endl;
    }

    {
        // using condition_variable without predicate
        std::mutex mutex;
        std::condition_variable cond_var;

        auto do_the_work = [&]() {
            std::cout << "Processing shared data\n";
        };
        auto waiting_for_work = [&]() {
            std::cout << "Worker: Watining for work.\n";
            std::unique_lock lock(mutex);
            cond_var.wait(lock);
            do_the_work();
            std::cout << "Work done.\n";
        };
        auto set_data_ready = [&]() {
            std::cout << "Sender: Data is ready.\n";
            cond_var.notify_one();
        };

        std::cout << std::endl;

        std::thread t1(waiting_for_work);
        std::thread t2(set_data_ready);

        t1.join(); t2.join();

        // If `cond_var.wait(lock)` is called at first, it works find.
        // But, if `cond_var.nofity_one()` is called before `cond_var.wait(lock)`, 
        // it makes infinite wait. So, the program cannot terminate.
        //
        // Conclusion: The predicate is a kind of memory for the stateless condition variable;
        // therefore, the wait call always checks the predicate at first.
        // Condition variables are victim to two known phenomena: lost wakeup and spurious wakeup

        std::cout << "-------------------------------------------------------" << std::endl << std::endl;
    }
}

int main()
{
    cond_var_exam();
}