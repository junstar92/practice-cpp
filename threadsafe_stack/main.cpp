#include <iostream>
#include <thread>
#include <mutex>
#include <memory>
#include <stack>

struct empty_stack : std::exception
{
    const char* what() const throw() {
        return "empty stack";
    };
};

template<typename T>
class threadsafe_stack
{
private:
    std::stack<T> st;
    mutable std::mutex m;

public:
    threadsafe_stack() {}
    threadsafe_stack(threadsafe_stack const& other) {
        std::lock_guard lock(other.m);
        st = other.st;
    }
    threadsafe_stack& operator=(threadsafe_stack const&) = delete;

    void push(T value) {
        std::lock_guard lock(m);
        st.push(std::move(value));
    }

    std::shared_ptr<T> pop() {
        std::lock_guard lock(m);
        if (st.empty()) throw empty_stack();

        std::shared_ptr<T> const ret(std::make_shared<T>(st.top())); // allocate return value before modifying stack
        st.pop();
        return ret;
    }

    void pop(T& value) {
        std::lock_guard lock(m);
        if (st.empty()) throw empty_stack();
        
        value = st.top();
        st.pop();
    }

    T top() const {
        std::lock_guard lock(m);
        return st.top();
    }

    bool empty() const {
        std::lock_guard lock(m);
        return st.empty();
    }
};



int main()
{
    threadsafe_stack<int> tst;

    std::thread t1 = std::thread([&](int s, int e) {
        for (int i = s; i < e; i++) {
            tst.push(i);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }, 0, 10);
    std::thread t2 = std::thread([&]() {
        tst.push(11);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        tst.pop();
        tst.push(22);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        tst.pop();
    });

    t1.join();
    t2.join();

    while (!tst.empty()) {
        std::cout << tst.top() << " ";
        tst.pop();
    }
    std::cout << std::endl;
}