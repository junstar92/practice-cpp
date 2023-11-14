#include <mutex>
#include <atomic>

namespace atomic {

namespace seq_cst {

class Singleton
{
public:
    static Singleton* get_instance() {
        Singleton* sin = instance.load();
        if (!sin) {
            std::lock_guard lock(mutex);
            sin = instance.load(std::memory_order_relaxed);
            if (!sin) {
                sin = new Singleton();
                instance.store(sin);
            }
        }
        volatile int dummy{};
        return sin;
    }

private:
    Singleton() = default;
    ~Singleton() = default;
    Singleton(Singleton const&) = delete;
    Singleton& operator=(Singleton const&) = delete;

    static std::atomic<Singleton*> instance;
    static std::mutex mutex;
};

std::atomic<Singleton*> Singleton::instance;
std::mutex Singleton::mutex;

}

namespace acq_rel {

class Singleton
{
public:
    static Singleton* get_instance() {
        Singleton* sin = instance.load(std::memory_order_acquire);
        if (!sin) {
            std::lock_guard lock(mutex);
            sin = instance.load(std::memory_order_release);
            if (!sin) {
                sin = new Singleton();
                instance.store(sin, std::memory_order_release);
            }
        }
        volatile int dummy{};
        return sin;
    }

private:
    Singleton() = default;
    ~Singleton() = default;
    Singleton(Singleton const&) = delete;
    Singleton& operator=(Singleton const&) = delete;

    static std::atomic<Singleton*> instance;
    static std::mutex mutex;
};

std::atomic<Singleton*> Singleton::instance;
std::mutex Singleton::mutex;

}

}