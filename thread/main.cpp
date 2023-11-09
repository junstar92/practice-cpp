#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <mutex>

void thread_basic()
{
    // Threads cannot be copied but can be moved.
    // The swap method perform a move when possible (refer to line 28)
    std::cout << std::boolalpha << std::endl;

    std::cout << "hardware_concurrency()= " << std::thread::hardware_concurrency() << std::endl;

    std::thread t1([]() { 
        std::cout << "t1 with id= " << std::this_thread::get_id() << std::endl;
    });
    std::thread t2([]() { 
        std::cout << "t2 with id= " << std::this_thread::get_id() << std::endl;
    });

    std::cout << std::endl;

    std::cout << "FROM MAIN: id of t1 " << t1.get_id() << std::endl;
    std::cout << "FROM MAIN: id of t2 " << t2.get_id() << std::endl;

    std::cout << std::endl;
    std::swap(t1, t2);

    std::cout << "FROM MAIN: id of t1 " << t1.get_id() << std::endl;
    std::cout << "FROM MAIN: id of t2 " << t2.get_id() << std::endl;

    std::cout << std::endl;

    std::cout << "FROM MAIN: id of main= " << std::this_thread::get_id() << std::endl;

    std::cout << std::endl;

    std::cout << "t1.joinable(): " << t1.joinable() << std::endl;

    std::cout << std::endl;

    t1.join();
    t2.join();

    std::cout << "t1.joinable(): " << t1.joinable() << std::endl;

    std::cout << "-------------------------------------------------------" << std::endl << std::endl;
}

namespace transfer_ownership
{

void some_function() {
    std::cout << "some_function\n";
}
void some_other_function() {
    std::cout << "some_other_function\n";
}

void test()
{
    std::thread t1(some_function);
    std::thread t2 = std::move(t1);

    // Thread cannot be copied.
    // The statement below starts a new thread that is associated with a temporary `std::thread` object.
    // The subsequent transfer of onwership into t1 doesn't require a call to `std::move()` to explicitly move ownership
    // because the owner is a temporary object - moving from temporaries is automatic and implicit.
    t1 = std::thread(some_other_function);
    
    // `t3` is default-constructed, which means that it's created without any associated thread of execution.
    std::thread t3;
    // Ownership of the thread currently assiciated with `t2` is transferred into `t3`
    // with an explicit call to `std::move()`, because `t2` is a named object.
    t3 = std::move(t2);
    // Current State at this point:
    // - `t1`: associated with the thread running `some_other_function`
    // - `t2`: no associated thread
    // - `t3`: associated with the thread running `some_function`

    // The statement below will terminate the program !!!
    // This move transfers ownership of the thread running `some_function` back to `t1` where it started.
    // But in this case, `t1` already had an associated thread (which was running `some_other_function`),
    // so, `std::terminate()` is called to terminate the program.
    // t1 = std::move(t3); 

    // Swap is OK.
    std::swap(t1, t3);
    

    if (t1.joinable()) t1.join();
    if (t2.joinable()) t2.join();
    if (t3.joinable()) t3.join();
}

}

namespace scoped_thread {

class scoped_thread
{
private:
    std::thread t;

public:
    explicit scoped_thread(std::thread t_)
    : t(std::move(t_)) {
        if (!t.joinable()) {
            throw std::logic_error("No thread");
        }
    }
    ~scoped_thread() {
        t.join();
    }
    scoped_thread(scoped_thread const&) = delete;
    scoped_thread& operator=(scoped_thread const&) = delete;
};

void test()
{
    scoped_thread t{std::thread([]() {
        std::cout << "function in thread\n";
    })};
    std::cout << "-------------------------------------------------------" << std::endl << std::endl;
}

}

namespace shared_data {
// Non-syncrhonized writing to `std::cout`
class Worker
{
private:
    std::string name;

public:
    Worker(std::string name_) : name{name_} {}
    void operator()() {
        for (int i = 1; i <= 3; i++) {
            // begin work
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            // end work
            std::cout << name << ": Work " << i << " done !!!" << std::endl;
        }
    }
};

void test()
{
    std::cout << "Boss: Let's start working." << std::endl << std::endl;

    std::thread herb = std::thread(Worker("Herb"));
    std::thread andrei = std::thread(Worker("  Andrei"));
    std::thread scott = std::thread(Worker("    Scott"));
    std::thread bjarne = std::thread(Worker("      Bjarne"));
    std::thread bart = std::thread(Worker("        Bart"));
    std::thread jenne = std::thread(Worker("          Jeene"));

    herb.join();
    andrei.join();
    scott.join();
    bjarne.join();
    bart.join();
    jenne.join();

    std::cout << std::endl << "Boss: Let's go home." << std::endl;
    std::cout << "-------------------------------------------------------" << std::endl << std::endl;
}

}

namespace mutexes {
// Mutex (mutual exclusion)
// - It ensures that only one thread can access a critical section at any one time.
std::mutex cout_mutex;

class Worker
{
private:
    std::string name;

public:
    Worker(std::string name_) : name{name_} {}
    void operator()() {
        for (int i = 1; i <= 3; i++) {
            // begin work
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            // end work
            cout_mutex.lock();
            std::cout << name << ": Work " << i << " done !!!" << std::endl;
            cout_mutex.unlock();
        }
    }
};

void test()
{
    std::cout << "Boss: Let's start working." << std::endl << std::endl;

    std::thread herb = std::thread(Worker("Herb"));
    std::thread andrei = std::thread(Worker("  Andrei"));
    std::thread scott = std::thread(Worker("    Scott"));
    std::thread bjarne = std::thread(Worker("      Bjarne"));
    std::thread bart = std::thread(Worker("        Bart"));
    std::thread jenne = std::thread(Worker("          Jeene"));

    herb.join();
    andrei.join();
    scott.join();
    bjarne.join();
    bart.join();
    jenne.join();

    std::cout << std::endl << "Boss: Let's go home." << std::endl;
    std::cout << "-------------------------------------------------------" << std::endl << std::endl;
}

}

int main()
{
    // thread baisc
    thread_basic();

    // transferring ownership of a thread
    transfer_ownership::test();

    // scoped thread (automatically join)
    scoped_thread::test();

    // shared data in thread
    shared_data::test();

    // thread with mutex
    mutexes::test();
}