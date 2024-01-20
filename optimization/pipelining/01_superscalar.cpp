#include <stdlib.h>

#include "benchmark/benchmark.h"

#define MCA_START asm volatile("# LLVM-MCA-BEGIN");
#define MCA_END asm volatile("# LLVM-MCA-END");

void BM_add(benchmark::State& state) {
    srand(1);
    const unsigned int N = state.range(0);
    std::vector<unsigned long> v1(N), v2(N);
    for (size_t i = 0; i < N; ++i) {
        v1[i] = rand();
        v2[i] = rand();
    }
    unsigned long* p1 = v1.data();
    unsigned long* p2 = v2.data();
    for (auto _ : state) {
        unsigned long a1 = 0;
        for (size_t i = 0; i < N; ++i) {
            // MCA_START
            a1 += p1[i] + p2[i];
            // MCA_END
        }
        benchmark::DoNotOptimize(a1);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(N*state.iterations());
    //state.SetBytesProcessed(N*sizeof(unsigned long)*state.iterations());
}

void BM_multiply(benchmark::State& state) {
    srand(1);
    const unsigned int N = state.range(0);
    std::vector<unsigned long> v1(N), v2(N);
    for (size_t i = 0; i < N; ++i) {
        v1[i] = rand();
        v2[i] = rand();
    }
    unsigned long* p1 = v1.data();
    unsigned long* p2 = v2.data();
    for (auto _ : state) {
        unsigned long a1 = 0;
        for (size_t i = 0; i < N; ++i) {
            a1 += p1[i] * p2[i];
        }
        benchmark::DoNotOptimize(a1);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(N*state.iterations());
    //state.SetBytesProcessed(N*sizeof(unsigned long)*state.iterations());
}

void BM_add_multiply(benchmark::State& state) {
    srand(1);
    const unsigned int N = state.range(0);
    std::vector<unsigned long> v1(N), v2(N);
    for (size_t i = 0; i < N; ++i) {
        v1[i] = rand();
        v2[i] = rand();
    }
    unsigned long* p1 = v1.data();
    unsigned long* p2 = v2.data();
    for (auto _ : state) {
        unsigned long a1 = 0, a2 = 0;
        for (size_t i = 0; i < N; ++i) {
            // MCA_START
            a1 += p1[i] + p2[i];
            a2 += p1[i] * p2[i];
            // MCA_END
        }
        benchmark::DoNotOptimize(a1);
        benchmark::DoNotOptimize(a2);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(N*state.iterations());
    //state.SetBytesProcessed(N*sizeof(unsigned long)*state.iterations());
}

void BM_add_multiply_sub_shift(benchmark::State& state) {
    srand(1);
    const unsigned int N = state.range(0);
    std::vector<unsigned long> v1(N), v2(N);
    for (size_t i = 0; i < N; ++i) {
        v1[i] = rand();
        v2[i] = rand();
    }
    unsigned long* p1 = v1.data();
    unsigned long* p2 = v2.data();
    for (auto _ : state) {
        unsigned long a1 = 0, a2 = 0;
        unsigned long a3 = 0, a4 = 0;
        for (size_t i = 0; i < N; ++i) {
            a1 += p1[i] + p2[i];
            a2 += p1[i] * p2[i];
            a3 += p1[i] << 2;
            a4 += p2[i] - p1[i];
        }
        benchmark::DoNotOptimize(a1);
        benchmark::DoNotOptimize(a2);
        benchmark::DoNotOptimize(a3);
        benchmark::DoNotOptimize(a4);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(N*state.iterations());
    //state.SetBytesProcessed(N*sizeof(unsigned long)*state.iterations());
}

void BM_instructions(benchmark::State& state) {
    srand(1);
    const unsigned int N = state.range(0);
    std::vector<unsigned long> v1(N), v2(N);
    for (size_t i = 0; i < N; ++i) {
        v1[i] = rand();
        v2[i] = rand();
    }
    unsigned long* p1 = v1.data();
    unsigned long* p2 = v2.data();
    for (auto _ : state) {
        unsigned long a1 = 0, a2 = 0;
        unsigned long a3 = 0, a4 = 0;
        unsigned long a5 = 0, a6 = 0;
        for (size_t i = 0; i < N; ++i) {
            a1 += p1[i] + p2[i];
            a2 += p1[i] * p2[i];
            a3 += p1[i] << 2;
            a4 += p2[i] - p1[i];
            a5 += (p2[i] << 1)*p2[i];
            a6 += (p2[i] - 3)*p1[i];
        }
        benchmark::DoNotOptimize(a1);
        benchmark::DoNotOptimize(a2);
        benchmark::DoNotOptimize(a3);
        benchmark::DoNotOptimize(a4);
        benchmark::DoNotOptimize(a5);
        benchmark::DoNotOptimize(a6);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(N*state.iterations());
    //state.SetBytesProcessed(N*sizeof(unsigned long)*state.iterations());
}

#define ARGS \
    ->Arg(1<<22)

BENCHMARK(BM_add) ARGS;
BENCHMARK(BM_multiply) ARGS;
BENCHMARK(BM_add_multiply) ARGS;
BENCHMARK(BM_add_multiply_sub_shift) ARGS;
BENCHMARK(BM_instructions) ARGS;

BENCHMARK_MAIN();