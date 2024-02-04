#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>

#include "benchmark/benchmark.h"

template<typename T>
void print_sorted1(std::vector<T> v) {
    std::sort(v.begin(), v.end());
    // disable the actual printing since we are not interested in benchmarking the I/O
    if (rand() < 0) for (T x: v) std::cout << x << std::endl;
}

template<typename T>
void print_sorted2(std::vector<T> const& v) {
    std::vector<T const*> vp;
    vp.reserve(v.size());
    for (T const& x : v) vp.push_back(&x);
    std::sort(vp.begin(), vp.end(), [](T const* a, T const* b) {
        return *a < *b;
    });
    // disable the actual printing since we are not interested in benchmarking the I/O
    if (rand() < 0) for (T const* p: vp) std::cout << *p << std::endl;
}

void BM_sort_cpy(benchmark::State& state) {
    size_t const N = state.range(0);
    std::vector<int> v0(N);
    for (int& x : v0) x = rand();
    std::vector<int> v(N);
    for (auto _ : state) {
        v = v0;
        print_sorted1(v);
    }
    state.SetItemsProcessed(state.iterations() * N);
}

void BM_sort_ptr(benchmark::State& state) {
    size_t const N = state.range(0);
    std::vector<int> v0(N);
    for (int& x : v0) x = rand();
    std::vector<int> v(N);
    for (auto _ : state) {
        v = v0;
        print_sorted2(v);
    }
    state.SetItemsProcessed(state.iterations() * N);
}

#define ARG \
    ->Arg(1UL << 10) \
    ->Arg(1UL << 20) \
    ->UseRealTime()

BENCHMARK(BM_sort_cpy) ARG;
BENCHMARK(BM_sort_ptr) ARG;

BENCHMARK_MAIN();