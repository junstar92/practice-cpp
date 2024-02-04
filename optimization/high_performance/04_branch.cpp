#include <cstdlib>

#include "benchmark/benchmark.h"

void f1(bool b, unsigned long x, unsigned long& s) {
    if (b) s += x;
}

void f2(bool b, unsigned long x, unsigned long& s) {
    s += b*x;
}

void BM_conditional(benchmark::State& state) {
    srand(1);
    const unsigned int N = state.range(0);
    std::vector<unsigned long> v1(N);
    std::vector<int> c1(N);
    for (size_t i = 0; i < N; ++i) {
        v1[i] = rand();
        c1[i] = rand() & 0x1;
    }
    unsigned long* p1 = v1.data();
    int* b1 = c1.data();
    for (auto _ : state) {
        unsigned long a1 = 0;
        for (size_t i = 0; i < N; ++i) {
            f1(b1[i], p1[i], a1);
        }
        benchmark::DoNotOptimize(a1);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(N*state.iterations());
}

void BM_branchless(benchmark::State& state) {
    srand(1);
    const unsigned int N = state.range(0);
    std::vector<unsigned long> v1(N);
    std::vector<int> c1(N);
    for (size_t i = 0; i < N; ++i) {
        v1[i] = rand();
        c1[i] = rand() & 0x1;
    }
    unsigned long* p1 = v1.data();
    int* b1 = c1.data();
    for (auto _ : state) {
        unsigned long a1 = 0;
        for (size_t i = 0; i < N; ++i) {
            f2(b1[i], p1[i], a1);
        }
        benchmark::DoNotOptimize(a1);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(N*state.iterations());
}


#define ARG \
    ->Arg(1<<22)

BENCHMARK(BM_conditional) ARG;
BENCHMARK(BM_branchless) ARG;

BENCHMARK_MAIN();