#include <unistd.h>
#include <atomic>
#include <memory>

#include "benchmark/benchmark.h"

#define REPEAT2(x) {x} {x}
#define REPEAT4(x) REPEAT2(x) REPEAT2(x)
#define REPEAT8(x) REPEAT4(x) REPEAT4(x)
#define REPEAT16(x) REPEAT8(x) REPEAT8(x)
#define REPEAT32(x) REPEAT16(x) REPEAT16(x)
#define REPEAT64(x) REPEAT32(x) REPEAT32(x)
#define REPEAT(x) REPEAT64(x)

template<typename T>
class ts_unique_ptr
{
std::atomic<T*> p_{nullptr};

public:
    ts_unique_ptr() = default;
    explicit ts_unique_ptr(T* p) : p_{p} {}

    ts_unique_ptr(ts_unique_ptr const&) = delete;
    ts_unique_ptr& operator=(ts_unique_ptr const&) = delete;

    ~ts_unique_ptr() {
        delete p_.load(std::memory_order_relaxed);
    }

    void publish(T* p) noexcept {
        p_.store(p, std::memory_order_release);
    }
    const T* get() const noexcept {
        return p_.load(std::memory_order_acquire);
    }
    const T& operator*() const noexcept {
        return *this->get();
    }
    ts_unique_ptr& operator=(T* p) noexcept {
        this->publish(p);
        return *this;
    }
};

struct A {
    int i;
    A(int i = 0) : i{i} {}
    A& operator=(A const& rhs) { 
        i = rhs.i;
        return *this;
    }
    volatile A& operator=(A const& rhs) volatile {
        i = rhs.i;
        return *this;
    }
};

ts_unique_ptr<A> tsup(new A(42));
void BM_ts_unique_ptr_deref(benchmark::State& state) {
    volatile A x;
    for (auto _ : state) {
        REPEAT(benchmark::DoNotOptimize(x = *tsup);)
    }
    state.SetItemsProcessed(64 * state.iterations());
}

std::shared_ptr<A> sp(new A(42));
void BM_shared_ptr_deref(benchmark::State& state) {
    volatile A x;
    for (auto _ : state) {
        REPEAT(benchmark::DoNotOptimize(x = *sp);)
    }
    state.SetItemsProcessed(64 * state.iterations());
}

void BM_atomic_shared_ptr_deref(benchmark::State& state) {
    volatile A x;
    for (auto _ : state) {
        REPEAT(benchmark::DoNotOptimize(x = *(std::atomic_load_explicit(
            &sp, std::memory_order_relaxed
        )));)
    }
    state.SetItemsProcessed(64 * state.iterations());
}

// T - type the pointer points to
// U - T with reference counter
//     U must have the following member functions:
//     void AddRef() - atomically increment reference count by 1
//     bool DelRef() - atomically decrement reference count by 1, return true if the counter dropped to 0
template<typename T, typename U = T>
class intr_shared_ptr
{
    struct get_ptr {
        std::atomic<U*>& aptr;
        U* p;
        get_ptr(std::atomic<U*>& ptr) : aptr(ptr), p() {
            static const timespec ns = { 0 , 1 };
            for (int i = 0; aptr.load(std::memory_order_relaxed) == (U*)(locked) || (p = aptr.exchange((U*)(locked), std::memory_order_acquire)) == (U*)(locked); ++i) {
                if (i == 8) {
                    i = 0;
                    nanosleep(&ns, NULL);
                }
            }
        }
        ~get_ptr() {
            aptr.store(p, std::memory_order_release);
        }
        static const uintptr_t locked = uintptr_t(-1);
    };

public:
    class shared_ptr {
    public:
        shared_ptr() : p_(nullptr) {}
        shared_ptr(shared_ptr const& x) : p_(x.p_) {
            if (p_) p_->AddRef();
        }
        ~shared_ptr() {
            if (p_ && p_->DelRef()) {
                delete p_;
            }
        }
        T& operator*() const { return *p_; }
        T* operator->() const { return p_; }
        explicit operator bool() const { return p_ != nullptr; }
        shared_ptr& operator=(shared_ptr const& x) {
            if (this == &x) return *this;
            if (p_ && p_->DelRef()) {
                delete p_;
            }
            p_ = x.p_;
            if (p_) p_->AddRef();
            return *this;
        }
        bool operator==(shared_ptr const& rhs) const {
            return p_ == rhs.p_;
        }
        bool operator!=(shared_ptr const& rhs) const {
            return p_ != rhs.p_;
        }
    
    private:
        friend class intr_shared_ptr;
        explicit shared_ptr(U* p) : p_(p) {
            if (p_) p_->AddRef();
        }
        void reset(U* p) {
            if (p_ == p) return;
            if (p_ && p_->DelRef()) {
                delete p_;
            }
            p_ = p;
            if (p_) p_->AddRef();
        }
        U* p_;
    };

    explicit intr_shared_ptr(U* p = nullptr) : p_(p) {
        if (p) p->AddRef();
    }
    explicit intr_shared_ptr(shared_ptr const& x) : p_() {
        if (x.p_) x.p_->AddRef();
        p_.store(x.p_, std::memory_order_relaxed);
    }
    explicit intr_shared_ptr(intr_shared_ptr const& x) : p_() {
        get_ptr px(x.p_);
        if (px.p) px.p->AddRef();
        p_.store(px.p, std::memory_order_relaxed);
    }
    ~intr_shared_ptr() {
        get_ptr p(p_);
        if (p.p && p.p->DelRef()) {
            delete p.p;
        }
        p.p = nullptr;
    }

    intr_shared_ptr& operator=(intr_shared_ptr const& x) {
        if (this == &x) return *this;
        
        U* pxp;
        {
            get_ptr px(x.p_);
            pxp = px.p;
            if (px.p) px.p->AddRef();
        }
        get_ptr p(p_);
        if (p.p && p.p->DelRef()) {
            delete p.p;
        }
        p.p = pxp;
        return *this;
    }
    explicit operator bool() const {
        return p_.load(std::memory_order_relaxed) != nullptr;
    }
    void reset(U* x) {
        get_ptr p(p_);
        if (p.p == x) return;
        if (p.p && p.p->DelRef()) {
            delete p.p;
        }
        p.p = x;
        if (x) x->AddRef();
    }
    void reset(shared_ptr const& x) {
        get_ptr p(p_);
        if (p.p == x.p_) return;
        if (p.p && p.p->DelRef()) {
            delete p.p;
        }
        p.p = x.p_;
        if (x.p_) x.p_->AddRef();
    }
    shared_ptr get() const {
        get_ptr p(p_);
        return shared_ptr(p.p);
    }
    bool compare_exchange_strong(shared_ptr& expected_ptr, shared_ptr const& new_ptr) {
        get_ptr p(p_);
        if (p.p == expected_ptr.p_) {
            if (p.p && p.p->DelRef()) {
                delete p.p;
            }
            p.p = new_ptr.p_;
            if (p.p) p.p->AddRef();
            return true;
        }
        else {
            expected_ptr.reset(p.p);
            return false;
        }
    }
    bool compare_exchange_strong(shared_ptr& expected_ptr, U* new_ptr) {
        get_ptr p(p_);
        if (p.p == expected_ptr.p_) {
            if (p.p && p.p->DelRef()) {
                delete p.p;
            }
            p.p = new_ptr;
            if (p.p) p.p->AddRef();
            return true;
        }
        else {
            expected_ptr.reset(p.p);
            return false;
        }
    }

private:
    mutable std::atomic<U*> p_;
};

static unsigned long B_count = 0;
struct B : public A {
    std::atomic<unsigned long> ref_cnt_;

    B(int i = 0) : A(i), ref_cnt_(0) {
        ++B_count;
    }
    ~B() { --B_count; }
    B(B const& x) = delete;
    B& operator=(B const& x) = delete;
    void AddRef() {
        ref_cnt_.fetch_add(1, std::memory_order_acq_rel);
    }
    bool DelRef() {
        return ref_cnt_.fetch_sub(1, std::memory_order_acq_rel) == 1;
    }
};

intr_shared_ptr<A, B> isp(new B(42));
void BM_intr_shared_ptr_deref(benchmark::State& state) {
    volatile A x;
    for (auto _ : state) {
        REPEAT(benchmark::DoNotOptimize(x = *isp.get());)
    }
    state.SetItemsProcessed(64 * state.iterations());
}

static const long numcpu = sysconf(_SC_NPROCESSORS_CONF);

#define ARGS \
    ->ThreadRange(1, numcpu) \
    ->UseRealTime()

BENCHMARK(BM_ts_unique_ptr_deref) ARGS;
BENCHMARK(BM_shared_ptr_deref) ARGS;
BENCHMARK(BM_atomic_shared_ptr_deref) ARGS;
BENCHMARK(BM_intr_shared_ptr_deref) ARGS;

BENCHMARK_MAIN();