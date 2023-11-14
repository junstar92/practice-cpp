#include <mutex>

namespace lock_guard {

std::mutex mutex;

class Singleton
{
public:
    static Singleton& get_instance() {
        std::lock_guard lock(mutex);
        if (!instance) {
            instance = new Singleton();
        }
        volatile int dummy{};
        return *instance;
    }

private:
    Singleton() = default;
    ~Singleton() = default;
    Singleton(Singleton const&) = delete;
    Singleton& operator=(Singleton const&) = delete;

    static Singleton* instance;
};

Singleton* Singleton::instance = nullptr;

}