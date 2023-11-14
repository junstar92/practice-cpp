namespace meyers_singleton {

/** 
 * When the program is compiled with optimization, the compiler removed the call
 * `Singleton::get_instance()` because the call has no effect. Therefore, we can get
 * very fast execution, but wrong performance numbers. By using volatile variable dummy,
 * the compiler is not allowed to optimize away the Singleton::get_instance() call.
 */
class Singleton
{
public:
    static Singleton& get_instance() {
        static Singleton instance;
        volatile int dummy{};
        return instance;
    }

private:
    Singleton() = default;
    ~Singleton() = default;
    Singleton(Singleton const&) = delete;
    Singleton& operator=(Singleton const&) = delete;
};

}