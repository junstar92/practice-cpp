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

unsigned long x{0};
void BM_incr(benchmark::State& state) {
    for (auto _ : state) {
        REPEAT(benchmark::DoNotOptimize(++x);)
    }
    state.SetItemsProcessed(32*state.iterations());
}

std::atomic<unsigned long> x1{0};
void BM_shared_incr(benchmark::State& state) {
    std::atomic<unsigned long>& x = x1;
    for (auto _ : state) {
        REPEAT(benchmark::DoNotOptimize(++x);)
    }
    state.SetItemsProcessed(32*state.iterations());
}

std::atomic<unsigned long> x2[1024];
void BM_false_shared_incr(benchmark::State& state) {
    std::atomic<unsigned long>& x = x2[state.thread_index()];
    for (auto _ : state) {
        REPEAT(benchmark::DoNotOptimize(++x);)
    }
    state.SetItemsProcessed(32*state.iterations());
}

static const long numcpu = sysconf(_SC_NPROCESSORS_CONF);
#define ARG \
    ->ThreadRange(1, numcpu) \
    ->UseRealTime()

BENCHMARK(BM_incr) ARG;
BENCHMARK(BM_shared_incr) ARG;
BENCHMARK(BM_false_shared_incr) ARG;

BENCHMARK_MAIN();