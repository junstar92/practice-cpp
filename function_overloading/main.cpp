#include <iostream>
#include <vector>

namespace ex1 {
    void f(int i) { std::cout << "f(int)\n"; }
    void f(long i) { std::cout << "f(long)\n"; }
    void f(double i) { std::cout << "f(double)\n"; }

    void test() {
        std::cout << "============= example 1\n";
        f(5);       // calls f(int)
        f(5l);      // calls f(long)
        f(5.0);     // calls f(double)
        f(5.0f);    // calls f(double) ; the conversion to double is preferred
        
        // f(5u);   // this call does not compile
        unsigned int i = 5u;
        f(static_cast<int>(i)); // calls f(int)
    }
}

namespace ex2 {
    void f(int i, int j) { std::cout << "f(int, int)\n"; }
    void f(long i, long j) { std::cout << "f(long, long)\n"; }
    void f(double i) { std::cout << "f(double)\n"; }

    void test() {
        std::cout << "============= example 2\n";
        f(5, 5);    // calls f(int, int)
        f(5l, 5l);  // calls f(long, long)
        /**
         * The first argument, int, matches the first overloaded exactly but could be converted to long if necessary.
         * The second argument, double, does not match either overloaded but could be converted to match both.
         * The first overload is a better match - it requires fewer argument conversions.
         */
        f(5, 5.0);  // calls f(int, int)

        /**
         * The first overload can be called, with a conversion on the second argument.
         * The second overload can also be made to work, with a conversion on the first argument.
         * So, this is an ambiguous overload, and this line will not compile.
         */
        //f(5, 5l);
    }
}

namespace ex3 {
    void f(int i) { std::cout << "f(int)\n"; }
    void f(long i, long j) { std::cout << "f(long, long)\n"; }
    void f(double i, double j = 0) { std::cout << "f(double, double = 0)\n"; }

    void test() {
        std::cout << "============= example 3\n";
        f(5);       // calls f(int)
        f(5l, 5);   // calls f(long, long)
        /**
         * This call seems straightforward enough with its two integer arguments, but this implicity is deceptive
         * - there are two overloads that accept two arguments, and in both overload cases, both arguments need conversions.
         * While the conversion from int to long may seem better than the one from int to double, C++ does not see it this way.
         * This call is ambiguous.
         */
        //f(5, 5);

        f(5.0);     // calls f(double, double = 0); it is a better match for the third overload, where it needs no conversion at all
        f(5.0f);    // calls f(double, double = 0); The conversion to double is better than that to int
        /**
         * The last call is again ambiguous - both conversions to double and to int are considered of equal weight,
         * thus the first and third overloads are equally good.
         * The second overload offers an exact match to the first parameter; however, there is no way to call that overload
         * without the second argument, so it is not even considered.
         */
        //f(5l);
    }
}

namespace ex4 {
    void f(int i) { std::cout << "f(int)\n"; }
    void f(long i) { std::cout << "f(long)\n"; }
    template<typename T>
    void f(T i) { std::cout << "f(T)\n"; }

    void test() {
        std::cout << "============= example 4\n";
        /**
         * If an exact match to a non-template function is found in the overload set,
         * it is always considered the best overload.
         */
        f(5);   // calls f(int)
        f(5l);  // calls f(long)
        /**
         * The last call has an argument of the double type that can be converted to int or long,
         * or subsituted for T to make the template instantiation an exact match.
         * Since there is no exactly matching non-template function, the template function instantiated
         * to an exact match is the next best overload and is selected.
         */
        f(5.0); // calls f(T)
    }
}

namespace ex5 {
    void f(int i) { std::cout << "f(int)\n"; }
    template<typename T>
    void f(T i) { std::cout << "f(T)\n"; }
    template<typename T>
    void f(T* i) { std::cout << "f(T*)\n"; }

    void test() {
        std::cout << "============= example 5\n";
        f(5);       // calls f(int)
        f(5l);      // calls f(T)
        int i = 0;
        f(&i);      // calls f(T*)
#define NULL 0
        f(NULL);    // calls f(int)
#undef NULL
        // The constant nullptr, despite having "ptr" in its name, it actually a constant value of type nullptr_t.
        // It is convertible to any pointer type but it is not of any pointer type.
        f(nullptr); // calls f(T) ; 
    }
}

namespace ex6 {
    void f(int i) { std::cout << "f(int)\n"; }
    void f(...) { std::cout << "f(...)\n"; }

    struct A {};
    struct B {
        operator int() const { return 0; }
    };

    void test() {
        std::cout << "============= example 6\n";
        f(5);       // calls f(int)
        f(5l);      // calls f(int)
        f(5.0);     // calls f(int)
        /**
         * The second function(called with any number of arguments of any type) is considered the choice of last resort.
         * A function with specific arguments that can be made to match the call with the right conversion is preferred.
         * This includes user-defined conversions.
         */
        A a;
        f(a);       // calls f(...)
        B b;
        // Only if there are no conversions that allow us to avoid calling the f(...) variadic function, then it has to be called.
        f(b);       // calls f(int)
    }
}

namespace ex7 {
    template<typename T>
    void f(T i, T* p) { std::cout << "f(T, T*)\n"; }
    void f(...) { std::cout << "f(...)\n"; }

    void test() {
        std::cout << "============= example 7\n";
        int i;
        f(5, &i);   // calls f(T, T*) ; T == int
        /**
         * When considering this call, we could deduce that T should be long from the first argument.
         * Alternatively, we could deduce that T should be int from the second argument.
         * This ambiguity leads to the failure of the type deduction process.
         * If this is the only overload available, neither option is chosen, and the program does not compile.
         * If more overloads exist, they are considered in turn, including possibly the doverload of last resort,
         * the f(...) variadic function wich is a viable option for calling f(long, int*).
         */
        f(5l, &i);  // calls f(...)

        /**
         * The ambiguous deduction can be resolved by explicitly specifying the template types,
         * which removes the need for type deduction.
         */
        f<int>(5l, &i);     // calls f(T, T*) ; T == int
        /**
         * Again, deduction of this call is not neccessary, as we know what T is.
         * Substituion proceeds in a straightforward way, and we end up with f(long, long*).
         * This function cannot be called with int* as the second argument since there is no valid conversion from int* to long*.
         * Thus, the program does not compile.
         */
        //f<long>(5l, &i);    // this call does not compile.
    }
}

namespace ex8 {
    void f(int i, int j = 1) { std::cout << "f(int, int=1)\n"; }
    template<typename T>
    void f(T i, T* p = nullptr) { std::cout << "f(T, T*)\n"; }

    void test() {
        std::cout << "============= example 8\n";
        int i;
        f(5);           // calls f(int, int=1)
        /**
         * This call is an exact match to the f(T, T*) template function, with T == long and
         * the default value of NULL for the second argument.
         * It doesn't matter at all that the type of that value is not long*.
         */
        f(5l);          // calls f(T, T*)
        f(5, &i);       // calls f(T, T*)
        //f(5l, &i);    // this call does not compile ; no known conversion from 'int*' to 'long*' for the second argument
    }
}

namespace ex9 {
    template<typename T>
    void f(T i, typename T::t& j) {
        std::cout << "f(T, T::t)\n";
    }
    template<typename T>
    void f(T i, T j) {
        std::cout << "f(T, T)\n";
    }

    struct A {
        struct t { int i; };
        t i;
    };

    void test() {
        std::cout << "============= example 9\n";
        A a{5};
        f(a, a.i);  // T == A
        /**
         * Look closely at the second call, f(5, 7).
         * The T type can be deduced as int from the first argument for both overloads.
         * Substituing int for T, however, yields something strange in the second argument of the first overade - int::t.
         * This, of course, would not compile - int is not a class and does not have any nested types.
         * In fact, we could expect that the first template overload will fail to compile for every T type that is not a class,
         * or that does not have a nested type called t.
         * However, this substitution failure does not mean that the entire program cannot compile. Instead, it is silently ignored,
         * and the overload that would otherwise be ill-formed is removed from the overload set. The overload resolution then 
         * continues as usual. Of course, we could discover that none of the overloads match the function call, and the program
         * will still not compile, but the error message will not mention anything about int::t being invalid: it'll just say that
         * there are no functions that can be called.
         */
        f(5, 7);    // T == int
    }
}

namespace ex10 {
    template<typename T>
    void f(T* i) {
        std::cout << "f(T*)\n";
    }
    template<typename T>
    void f(int T::* p) {
        std::cout << "f(T::*)\n";
    }

    struct A { int i; };

    void test() {
        std::cout << "============= example 10\n";
        A a{3};
        f(&a.i);    // calls f(T*)
        f(&A::i);   // calls f(T::*)
    }
}

namespace ex11 {
    template<size_t N>
    void f(char(*)[N % 2] = nullptr) {
        std::cout << "N=" << N << " is odd; calls f(char(*)[N % 2]\n";
    }
    template<size_t N>
    void f(char(*)[1 - N % 2] = nullptr) {
        std::cout << "N=" << N << " is even; calls f(char(*)[1 - N % 2]\n";
    }
    template<typename T, size_t N = T::N>
    void f(T t, char(*)[N % 2] = nullptr) {
        std::cout << "N=" << N << " is odd; calls f(T, char(*)[N % 2]\n";
    }
    template<typename T, size_t N = T::N>
    void f(T t, char(*)[1 - N % 2] = nullptr) {
        std::cout << "N=" << N << " is even; calls f(T, char(*)[1 - N % 2]\n";
    }

    struct A { enum {N = 5}; };
    struct B { enum {N = 8}; };

    void test() {
        std::cout << "============= example 11\n";
        f<5>();
        f<8>();
        A a;
        B b;
        f(a);
        f(b);
    }
}

namespace ex12 {
    template<typename T>
    void f(T) {
        std::cout << "f(T) " << sizeof(T::i) << std::endl;
    }
    void f(...) {
        std::cout << "f(...)\n";
    }

    struct A {
        typedef int t;
        t i;
    };

    void test() {
        std::cout << "============= example 12\n";
        /**
         * The first call does not compile:
         * SFINAE does not suppress errors in the body of the function!
         */
        // f(0);
        A a{5};
        f(a);
    }
}

namespace ex13 {
    /**
     * If we try to call this function with an argument that does not define the nested type, value_type,
     * the function call will not compile (assuming we have no other overloadings).
     */
    template<typename T>
    bool find(const T& cont, typename T::value_type val) {
        for (auto x : cont) {
            if (x == val) return true;
        }
        return false;
    }
    /**
     * In this template, the return type is the common type of the two template parameter types.
     * But what if the the template arguments are such that the types U and V have no common type ?
     * Then the type expression std::common_type<U, V> is invalid and the type substitution fails.
     */
    template<typename U, typename V>
    std::common_type_t<U, V> compute(U u, V v) {
        return u + v;
    }
    /**
     * In this template, the substitution failure may occur in the return type, but we use trailing
     * return type so that we can directly check whether the expression *p compiles (or, more formally, is invalid).
     * If it is, the type of the result is the return type. Otherwise, the substitution fails.
     */
    template<typename T>
    auto process(T p) -> decltype(*p) {
        return *p;
    }

    void test() {
        std::cout << "============= example 13\n";
        std::vector<int> v{ 1, 1, 2, 3, 5, 8, 13, 21 };
        std::cout << find(v, 2) << " " << find(v, 7) << std::endl;
        
        int a[] = { 1, 2, 3 };
        // find(a, 2); // does not compile - subsitution failure

        std::cout << compute(1, 2.5) << std::endl;
        //compute(v, 1);  // does not compile - subsitution failure

        std::cout << process(v.begin()) << std::endl;
        //process(0);     // does not compile - subsitution failure
    }
}

namespace ex14 {
    /**
     * In this template, we decided to accept a value of any type. This is not necessarily wrong in itself,
     * but the body of our template function is written in the assumption that the container type T has
     * the nested type value_type and that this type is comparable with the type V. If we call the function with
     * a wrong argument, the call will still compile because nothing particular is required of the argument types
     * by the substitution that happens in the declaration of the template. But then we get a syntax error in the
     * body of the template itself, rather than at the call site.
     */
    template<typename T, typename V>
    bool find(const T& cont, V val) {
        for (typename T::value_type x : cont) {
            if (x == val) return true;
        }
        return false;
    }
    /**
     * This template function can be called for any two arguments, but then it won't compile unless there is a common
     * type for both.
     */
    template<typename U, typename V>
    auto compute(U u, V v) {
        std::common_type_t<U, V> res = (u > v) ? u : v;
        return res;
    }
    template<typename T>
    auto process(T p) -> decltype(*p) {
        return *p;
    }

    void test() {
        std::cout << "============= example 14\n";
        std::vector<int> v{ 1, 1, 2, 3, 5, 8, 13, 21 };
        std::cout << find(v, 2) << " " << find(v, 7) << std::endl;
        
        int a[] = { 1, 2, 3 };
        //find(a, 2); // does not compile - syntax error

        std::cout << compute(1, 2.5) << std::endl;
        //compute(v, 1);  // does not compile - syntax error

        std::cout << process(v.begin()) << std::endl;
        //process(0);     // does not compile - subsitution failure
    }
}

namespace ex15 {
    /**
     * This function takes two pointers (or any other pointer-like bojects that can be dereferenced) and
     * returns the Boolean result of comparing the values they point to. In order for the body of the function
     * to compile, both arguments must be something that can be dereferenced.
     * Furthermore, the results of dereferencing them must be comparable for equality. Finally, the result of
     * the comparison must be convertible to bool. The trailing return type declaration is unnecessary: we could
     * have just declared the function to return bool. But it does have an effect: it moves a possible substitution
     * failure from the body of the function to its declaration, where it becomes an SFINAE failure.
     */
    template<typename U, typename V>
    auto compare(U pu, V pv) -> decltype(bool(*pu == *pv)) {
        return *pu < *pv;
    }

    void test() {
        std::cout << "============= example 15\n";
        int i = 1;
        double d = 2.5;
        std::cout << compare(&i, &d) << std::endl;
        //compare(i, 1); // does not compile - substitution failed
    }
}

int main()
{
    ex1::test();
    ex2::test();
    ex3::test();
    ex4::test();
    ex5::test();
    ex6::test();
    ex7::test();
    ex8::test();
    ex9::test();
    ex10::test();
    ex11::test();
    ex12::test();
    ex13::test();
    ex14::test();
    ex15::test();
}