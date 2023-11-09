#include <iostream>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <queue>
#include <thread>

template<typename T>
class threadsafe_queue
{
private:
    mutable std::mutex mutex;
    std::queue<T> queue;
    std::condition_variable cond_var;

public: 
    threadsafe_queue() {}
    threadsafe_queue(threadsafe_queue const& other) {
        std::lock_guard lock(other.mutex);
        queue = other.queue;
    }

    void push(T value) {
        std::lock_guard lock(mutex);
        queue.push(value);
        cond_var.notify_one();
    }

    void wait_and_pop(T& value) {
        std::unique_lock lock(mutex);
        cond_var.wait(lock, [this]() { return !queue.empty(); });

        value = queue.front();
        queue.pop();
    }

    std::shared_ptr<T> wait_and_pop() {
        std::unique_lock lock(mutex);
        cond_var.wait(lock, [this]() { return !queue.empty(); });

        std::shared_ptr<T> ret{std::make_shared<T>(queue.front())};
        queue.pop();

        return ret;
    }

    bool try_pop(T& value) {
        std::lock_guard lock(mutex);
        if (queue.empty()) {
            return false;
        }

        value = queue.front();
        queue.pop();

        return true;
    }

    std::shared_ptr<T> try_pop() {
        std::lock_guard lock(mutex);
        if (queue.empty()) {
            return std::shared_ptr<T>{};
        }

        std::shared_ptr<T> ret{std::make_shared<T>(queue.front())};
        queue.pop();

        return ret;
    }

    bool empty() const {
        std::lock_guard lock(mutex);
        return queue.empty();
    }
};



int main()
{
    threadsafe_queue<int> tq;

    std::thread t1 = std::thread([&](int s, int e) {
        for (int i = s; i < e; i++) {
            tq.push(i);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }, 0, 10);
    std::thread t2 = std::thread([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(3));
        tq.push(11);
        tq.wait_and_pop();
        tq.push(22);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        tq.try_pop();
    });

    t1.join();
    t2.join();

    while (!tq.empty()) {
        std::cout << *tq.wait_and_pop() << " ";
    }
    std::cout << std::endl;
}