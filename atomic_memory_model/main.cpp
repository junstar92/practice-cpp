#include <iostream>
#include <atomic>
#include <string>
#include <thread>

/*
Kind of Atomic Operation
- Read: `memory_order_acquire`, `memory_order_consume`
- Write: `memory_order_release`
- Read-modify-write: `memory_order_acq_rel`, `memory_order_seq_cst`
`memory_order_relaxed` does not fit in this taxonomy.

Different Synchronization and Ordering Constraints
- Sequential consistency: `memory_order_seq_cst`
- Acquire-release: `memory_order_consume`, `memory_order_acquire`, `memory_order_release`, `memory_order_acq_rel`
- Relaxed: `memory_order_relaxed`
*/

void sequential_consistency()
{
    std::cout << "------------ Sequential Consistency Example -----------" << std::endl;
    // Example for Sequential Consistency (producer and consumer thread sychronization)
    //
    // The key for sequential consistency is that all operations on all threads obey a universal clock.
    // But, the sequential consistency comes with a price to synchronize threads.
    {
        std::string work;
        std::atomic<bool> ready{false};

        auto consumer = [&]() {
            while (!ready.load()) {}
            std::cout << work << std::endl;
        };
        auto producer = [&]() {
            work = "done";
            ready = true;
        };

        std::thread prod(producer);
        std::thread con(consumer);
        prod.join();
        con.join();

        // 1. `work = "done"` is sequenced-before `ready = true`
        //    => `work = "done" happens-before `ready = true`
        // 2. `while(!ready.load) {}` is sequenced-before `std::cout << work << std::endl`
        //    => `while(!ready.load) {}` is happens-before `std::cout << work << std::endl`
        // 3. `ready = true` synchronizes-with `while (!ready.load()) {}`
        //    => `ready = true` inter-thread happends-before `while (!ready.load()) {}`
        //    => `ready = true` happends-before `while (!ready.load()) {}`
        //
        // In sequential consistency, a thread sees the operations of another thread and 
        // therefore of all other threads in the same order.
    }

    {
        std::atomic<bool> x, y;
        std::atomic<int> z;

        auto write_x = [&x]() {
            x.store(true, std::memory_order_seq_cst);
        };
        auto write_y = [&y]() {
            y.store(true, std::memory_order_seq_cst);
        };
        auto read_x_then_y = [&]() {
            while (!x.load(std::memory_order_seq_cst));
            if (y.load(std::memory_order_seq_cst))
                ++z;
        };
        auto read_y_then_x = [&]() {
            while (!y.load(std::memory_order_seq_cst));
            if (x.load(std::memory_order_seq_cst))
                ++z;
        };

        x = false;
        y = false;
        z = 0;
        std::thread t1(write_x);
        std::thread t2(write_y);
        std::thread t3(read_x_then_y);
        std::thread t4(read_y_then_x);
        t1.join(); t2.join(); t3.join(); t4.join();
        assert(z.load() != 0);
        std::cout << "z : " << z << std::endl; // exepcted: 1 or 2
    }
    
    std::cout << "-------------------------------------------------------" << std::endl << std::endl;
}

class SpinLock
{
private:
    std::atomic_flag flag = ATOMIC_FLAG_INIT;

public:
    void lock() {
        while (flag.test_and_set(std::memory_order_acquire));
    }

    void unlock() {
        flag.clear(std::memory_order_release);
    }
};

void acquire_release_semantic()
{
    std::cout << "----------- Acquire-Release Semantic Example ----------" << std::endl;
    // Example for Acquire-Release Semantic
    //
    // There is no global synchronization between threads in the acquire-release semantic
    // : there is only synchronization between atomic operations on the same atomic variable.
    //
    // The acquire-release semantic is based on on fundamental idea
    // : a release operation synchronizes with an acquire operation on the same atomic
    //   and establishes an ordering constraint.
    // => All read and write operations cannot be moved after a release operation,
    //    and all read and write operations cannot be moved before an acquire operation.
    //    

    {
        // A Spinlock with acquire-realse semantic
        SpinLock spin;

        auto work_on_resource = [&]() {
            spin.lock();
            // critical section
            spin.unlock();
        };

        std::thread t1(work_on_resource);
        std::thread t2(work_on_resource);

        t1.join();
        t2.join();
    }

    // producer-consumer pattern using acquire-release semantic 1
    {
        std::vector<int> my_shared_work;
        std::atomic<bool> data_produced{false};
        std::atomic<bool> data_consumed{false};

        auto data_producer = [&]() {
            my_shared_work = {1, 0, 3};
            data_produced.store(true, std::memory_order_release);
        };
        auto delivery_boy = [&]() {
            while (!data_produced.load(std::memory_order_acquire));
            data_consumed.store(true, std::memory_order_release);
        };
        auto data_consumer = [&]() {
            while (!data_consumed.load(std::memory_order_acquire));
            my_shared_work[1] = 2;
        };

        std::thread t1(data_consumer);
        std::thread t2(delivery_boy);
        std::thread t3(data_producer);

        t1.join();
        t2.join();
        t3.join();

        for (auto v : my_shared_work) {
            std::cout << v << " ";
        }
        std::cout << std::endl;

        // Two important observations:
        // 1. Thread t2 waits in line 114, until thread t3 sets `data_produced` to `true` (line 111)
        // 2. Thread t1 waits in line 118, until therad t2 sets `data_consumed` to `true` (line 115)
    }
    
    // producer-consumer pattern using acquire-release semantic 2
    // : it doesn't imply a total ordering
    {
        std::atomic<bool> x, y;
        std::atomic<int> z;

        auto write_x = [&x]() {
            x.store(true, std::memory_order_release);
        };
        auto write_y = [&y]() {
            y.store(true, std::memory_order_release);
        };
        auto read_x_then_y = [&]() {
            while (!x.load(std::memory_order_acquire));
            if (y.load(std::memory_order_acquire))
                ++z;
        };
        auto read_y_then_x = [&]() {
            while (!y.load(std::memory_order_acquire));
            if (x.load(std::memory_order_acquire))
                ++z;
        };

        x = false;
        y = false;
        z = 0;
        std::thread t1(write_x);
        std::thread t2(write_y);
        std::thread t3(read_x_then_y);
        std::thread t4(read_y_then_x);
        t1.join(); t2.join(); t3.join(); t4.join();
        assert(z.load() != 0); // it can fire because there's no happens-before relationship to force an ordering.
        std::cout << "z : " << z << std::endl;
    }

    // producer-consumer pattern using acquire-release semantic 3
    {
        std::atomic<bool> is_ready{false};
        int data = 0;

        auto producer = [&]() {
            data = 10;
            is_ready.store(true, std::memory_order_release);
        };
        auto consumer = [&]() {
            while (!is_ready.load(std::memory_order_acquire));
            std::cout << "Data: " << data << std::endl; // expected: 10
        };

        std::thread t1(consumer);
        std::thread t2(producer);
        t1.join(); t2.join();
    }
    
    std::cout << "-------------------------------------------------------" << std::endl << std::endl;
}

/* std::memory_order_consume
- It is extremely hard to understand.
- No compiler supports it currently (this may change in the future).


*/

void memory_order_consume()
{
    std::cout << "------ Release-Acquire & Release-Consume Ordering -----" << std::endl;
    // Example for Release-Acquire Ordering
    {
        std::atomic<std::string*> ptr;
        int data;
        std::atomic<int> atomic_data;

        auto producer = [&]() {
            std::string* p = new std::string("C++11");
            data = 2011;
            atomic_data.store(2014, std::memory_order_relaxed);
            ptr.store(p, std::memory_order_release);
        };
        auto consumer = [&]() {
            std::string* p2;
            while (!(p2 = ptr.load(std::memory_order_acquire)));
            std::cout << "*p2: " << *p2 << std::endl;    // expected: C++11
            std::cout << "data: " << data << std::endl;  // expected: 2011
            std::cout << "atomic_data: " << atomic_data.load(std::memory_order_relaxed) << std::endl; // expected: 2014
        };

        std::thread t1(producer);
        std::thread t2(consumer);

        t1.join();
        t2.join();

        std::cout << std::endl;

        // The store operation uses `std::memory_order_release`
        // and the load operation uses `std::memory_order_acquire`.
        // So, The store operation on line 253 synchronizes-with the load operation in line 257.
        // => This store/load operation is the synchronization.
        //    The release-acquire ordering guarantees that the results of all operations
        //    before the store operation are available after the load operation.
        //    So also, the release-acquire operation orders access to the non-atomic variable `data` (line 251)
        //    and the atomic variable `atomic_data`.
    }

    // Example for Release-Consume Ordering (Data dependencies with `std::memory_order_consume`)
    // - undefined behavior
    {
        std::atomic<std::string*> ptr;
        int data;
        std::atomic<int> atomic_data;

        auto producer = [&]() {
            std::string* p = new std::string("C++11");
            data = 2011;
            atomic_data.store(2014, std::memory_order_relaxed);
            ptr.store(p, std::memory_order_release);
        };
        auto consumer = [&]() {
            std::string* p2;
            while (!(p2 = ptr.load(std::memory_order_consume)));
            std::cout << "*p2: " << *p2 << std::endl;    // undefined result
            std::cout << "data: " << data << std::endl;  // undefined result
            std::cout << "atomic_data: " << atomic_data.load(std::memory_order_relaxed) << std::endl; // undefined result
        };

        std::thread t1(producer);
        std::thread t2(consumer);

        t1.join();
        t2.join();
    }

    // another example for memory_order_consume
    {
        struct X {
            int i;
            std::string s;
        };
        std::atomic<X*> p;
        std::atomic<int> a;

        auto create_x = [&]() {
            X* x = new X;
            x->i = 42;
            x->s = "hello";
            a.store(99, std::memory_order_relaxed);
            p.store(x, std::memory_order_relaxed);
        };
        auto use_x = [&]() {
            X* x;
            while (!(x = p.load(std::memory_order_consume)))
                std::this_thread::sleep_for(std::chrono::microseconds(1));
            assert(x->i == 42);
            assert(x->s == "hello");
            assert(a.load(std::memory_order_consume) == 99); // it may occur segmentation fault
        };

        std::thread t1(create_x);
        std::thread t2(use_x);
        t1.join(); t2.join();
    }
    
    std::cout << "-------------------------------------------------------" << std::endl << std::endl;
}

void relax_semantic()
{
    std::cout << "----- No synchronization and ordering constraints -----" << std::endl;
    // The relaxed semantic is the other end of the spectrum.
    // It is the weakest of all memory models and only guarantees the modification order of atomics.
    // This means all modifications on an atomic happen in some particular total order.

    {
        std::atomic<int> count{0};

        auto add = [&]() {
            for (int n = 0; n < 1000; ++n) {
                count.fetch_add(1, std::memory_order_relaxed);
            }
        };

        std::vector<std::thread> v;
        for (int i = 0; i < 10; i++) {
            v.emplace_back(add);
        }
        for (auto& t : v) {
            t.join();
        }
        std::cout << "Final counter value is " << count << std::endl; // expected 10000

        // In line 354, the atomic number count is incremented using the relaxed semantic,
        // so we have a guarantee that the operation is atomic.
        // The `fetch_add` operation stabilshes an ordering on `count`.
        // The function `add` (line 352 ~ 356) is the work package of the threads.
        //
        // There is a happens-before relation between the increment operation in line 354
        // and reading of the counter `count` in line 365.
    }

    std::cout << "-------------------------------------------------------" << std::endl << std::endl;
}

int main()
{
    // test for std::memory_order_seq_cst
    sequential_consistency();
    // test for std::memory_order_release and std::memory_order_acquire
    acquire_release_semantic();
    // test for std::memory_order_consume instead of std::memory_order_release
    memory_order_consume();
    // test for std::memory_order_relaxed
    relax_semantic();

    return 0;
}