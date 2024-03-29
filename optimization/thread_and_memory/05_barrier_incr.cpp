#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <atomic>

#include "benchmark/benchmark.h"

#define REPEAT2(x) x x
#define REPEAT4(x) REPEAT2(x) REPEAT2(x)
#define REPEAT8(x) REPEAT4(x) REPEAT4(x)
#define REPEAT16(x) REPEAT8(x) REPEAT8(x)
#define REPEAT32(x) REPEAT16(x) REPEAT16(x)
#define REPEAT(x) REPEAT32(x)

std::atomic<unsigned long> x(0);

void BM_relaxed(benchmark::State& state) {
    for (auto _ : state) {
        REPEAT(benchmark::DoNotOptimize(x.fetch_add(1, std::memory_order_relaxed));)
    }
    state.SetItemsProcessed(32*state.iterations());
}

void BM_release(benchmark::State& state) {
    for (auto _ : state) {
        REPEAT(benchmark::DoNotOptimize(x.fetch_add(1, std::memory_order_release));)
    }
    state.SetItemsProcessed(32*state.iterations());
}

void BM_acquire(benchmark::State& state) {
    for (auto _ : state) {
        REPEAT(benchmark::DoNotOptimize(x.fetch_add(1, std::memory_order_acquire));)
    }
    state.SetItemsProcessed(32*state.iterations());
}

void BM_acq_rel(benchmark::State& state) {
    for (auto _ : state) {
        REPEAT(benchmark::DoNotOptimize(x.fetch_add(1, std::memory_order_acq_rel));)
    }
    state.SetItemsProcessed(32*state.iterations());
}

void BM_seq_cst(benchmark::State& state) {
    for (auto _ : state) {
        REPEAT(benchmark::DoNotOptimize(x.fetch_add(1, std::memory_order_seq_cst));)
    }
    state.SetItemsProcessed(32*state.iterations());
}

#define ARG \
    ->Threads(2) \
    ->UseRealTime()

BENCHMARK(BM_relaxed) ARG;
BENCHMARK(BM_release) ARG;
BENCHMARK(BM_acquire) ARG;
BENCHMARK(BM_acq_rel) ARG;
BENCHMARK(BM_seq_cst) ARG;

BENCHMARK_MAIN();