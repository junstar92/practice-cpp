#include <iostream>
#include "benchmark/benchmark.h"

#define REPEAT2(x) x x
#define REPEAT4(x) REPEAT2(x) REPEAT2(x)
#define REPEAT8(x) REPEAT4(x) REPEAT4(x)
#define REPEAT16(x) REPEAT8(x) REPEAT8(x)
#define REPEAT32(x) REPEAT16(x) REPEAT16(x)
#define REPEAT(x) REPEAT32(x)

/** What is Wrong with a Virtual Function ?
 * The problem of the virtual function is the performance overhead.
 * A virtual function call can be several times more expensive than a non-virtual call, more for very simple functions
 * that would have been inlined were they not virtual (a virtual function can never be inlined). This difference can
 * be measured with a microbenchmark.
 */

namespace no_polymorphism {

class A
{
public:
    A() : i_{0} {}
    void f(int i) { i_ += i; }
    int get() const { return i_; }

protected:
    int i_;
};

}

namespace dynamic_polymorphism
{

class B
{
public:
    B() : i_{0} {}
    virtual ~B() {}
    virtual void f(int i) = 0;
    int get() const { return i_; }

protected:
    int i_;
};

class D : public B {
public:
    void f(int i) { i_ += i; }
};

}

namespace static_polymorphism
{
/** Introducing CRTP
 * The first change is that the base class is now a class template. The derived class still inherits from
 * the base class, but now from the specific instantiation of the base class template - on its own.
 * Class B is instantiated on class D, and class D inherits from that instantiation of class B, which is
 * instantiated on class D, which in herits from class B, which ... that's recursion in action.
 * 
 * What is the motivation for this mind-twisting pattern ? Consider that now the base class has compile-time
 * information about the derived class. Therefore, what used to be a virtual function call can now be bound to
 * the right function at compile time.
 * 
 * The `f()` function call itself can still be done on the base class pointer. There is no indirection and
 * no overhead for the virtual call. The compile can, at compile time, track the call all the way to the
 * actual function called, and even inline it.
 */
template<typename D>
class B
{
public:
    B() : i_{0} {}
    virtual ~B() {}
    void f(int i) { static_cast<D*>(this)->f(i); }
    int get() const { return i_; }

protected:
    int i_;
};

class D : public B<D>
{
public:
    void f(int i) { i_ += i; }
};

}

void BM_none(benchmark::State& state)
{
    no_polymorphism::A* a = new no_polymorphism::A;
    int i = 0;

    for (auto _ : state) {
        REPEAT(a->f(++i);)
    }
    benchmark::DoNotOptimize(a->get());
    state.SetItemsProcessed(32 * state.iterations());
    delete a;
}

void BM_dynamic(benchmark::State& state)
{
    dynamic_polymorphism::B* b = new dynamic_polymorphism::D;
    int i = 0;

    for (auto _ : state) {
        REPEAT(b->f(++i);)
    }
    benchmark::DoNotOptimize(b->get());
    state.SetItemsProcessed(32 * state.iterations());
    delete b;
}

void BM_static(benchmark::State& state)
{
    static_polymorphism::B<static_polymorphism::D>* b = new static_polymorphism::D;
    int i = 0;

    for (auto _ : state) {
        REPEAT(b->f(++i);)
    }
    benchmark::DoNotOptimize(b->get());
    state.SetItemsProcessed(32*state.iterations());
    delete b;
}

BENCHMARK(BM_none);
BENCHMARK(BM_dynamic);
BENCHMARK(BM_static);

BENCHMARK_MAIN();