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

ts_unique_ptr<A> p(new A(42));

void BM_ptr_deref(benchmark::State& state) {
    volatile A x;
    for (auto _ : state) {
        REPEAT(benchmark::DoNotOptimize(x = *p);)
    }
    state.SetItemsProcessed(32 * state.iterations());
}

A* q(new A(7));

void BM_ptr_assign(benchmark::State& state) {
    for (auto _ : state) {
        REPEAT(benchmark::DoNotOptimize(p = q);)
    }
    state.SetItemsProcessed(32 * state.iterations());
}

std::unique_ptr<A> rp(new A(42));

void BM_raw_ptr_deref(benchmark::State& state) {
    volatile A x;
    for (auto _ : state) {
        REPEAT(benchmark::DoNotOptimize(x = *rp);)
    }
    state.SetItemsProcessed(32 * state.iterations());
}

static const long numcpu = sysconf(_SC_NPROCESSORS_CONF);

#define ARGS \
    ->ThreadRange(1, numcpu) \
    ->UseRealTime()

BENCHMARK(BM_ptr_deref) ARGS;
BENCHMARK(BM_ptr_assign) ARGS;
BENCHMARK(BM_raw_ptr_deref) ARGS;

BENCHMARK_MAIN();