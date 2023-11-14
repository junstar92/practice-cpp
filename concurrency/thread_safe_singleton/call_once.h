#include <mutex>

namespace call_once {

class Singleton
{
public:
    static Singleton& get_instance() {
        std::call_once(init_instance_flag, &Singleton::init_singleton);
        volatile int dummy{};
        return *instance;
    }

private:
    Singleton() = default;
    ~Singleton() = default;
    Singleton(Singleton const&) = delete;
    Singleton& operator=(Singleton const&) = delete;

    static Singleton* instance;
    static std::once_flag init_instance_flag;

    static void init_singleton() {
        instance = new Singleton;
    }
};

Singleton* Singleton::instance = nullptr;
std::once_flag Singleton::init_instance_flag;

}