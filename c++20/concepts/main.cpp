#include <iostream>
#include <vector>
#include <algorithm>
#include <type_traits>

namespace ex1 {
    /**
     * The first way to specify a constraint is by writing a requires clause that has the form:
     *   requires(constant-boolean-expression)
     * The keyword 'requires' and the constant (compile-time) expression in parenthese must appear either immediately
     * after the template parameters or as the last element of the function declaration:
     *   template<typename T> requires(sizeof(T) == 8) void f();
     *   template<typename T> void g(T p) requires(sizeof(*p) < 8);
     */
    template<typename T>
    requires(sizeof(T) == 8)
    void f(T&&) {
        std::cout << "f(T), sizeof(T) = " << sizeof(T) << " == 8\n";
    }
    template<typename T>
    void f(T&& t)
    requires(sizeof(T) < 8) {
        std::cout << "f(T), sizeof(T) = " << sizeof(T) << " < 8\n";
    }
    template<typename T>
    void g(T p)
    requires(sizeof(*p) < 8) {
        std::cout << "f(T), sizeof(*T) = " << sizeof(*p) << " < 8\n";
    }

    template<typename T>
    constexpr bool valid_type() {
        return sizeof(T) == 8 && alignof(T) == 8 && std::is_default_constructible_v<T>;
    }
    template<typename T>
    requires(valid_type<T>())
    void f() {
        std::cout << "f(T) with valid_type<T>()\n";
    }

    struct A {
        double i;
    };

    void test() {
        std::cout << "============= example 1\n";
        f(0.0);
        f(0);
        int i;
        double d;
        g(&i);
        //g(&d); // constraint fails
        f<A>();
    }
}

namespace ex2 {
    template<typename T, typename P>
    void f(T i, P p) requires( requires { i = *p; } ) {
        std::cout << "f(T, T*)\n";
    }
    template<typename T, typename P>
    void f(T i, P p) requires( requires { i.*p; } ) {
        std::cout << "f(T, T::*)\n";
    }
    template<typename T, typename P>
    requires( requires(T a, P b) { a/b; } )
    void f(T i, P p) {
        std::cout << "f(T, P)\n";
    }

    struct A {
        int i;
        int f() { return 0; }
    };

    void test() {
        std::cout << "============= example 2\n";
        A a;
        f(0, &a.i);
        f(a, &A::i);
        f(1, 2.0);
    }
}

namespace ex3 {
    /**
     * Concepts are simply a named set of requirements. In a way, they are similar to constexpr functions,
     * except they operate on types, not values.
     */
    template<typename R> concept Range = requires(R r) {
        std::begin(r);
        std::end(r);
    };

    template<typename R>
    requires(Range<R>)
    void sort1(R&& r) {
        std::cout << "Sorting range 1\n";
        std::sort(std::begin(r), std::end(r));
    }
    template<Range R>
    void sort2(R&& r) {
        std::cout << "Sorting range 2\n";
        std::sort(std::begin(r), std::end(r));
    }
    void sort3(Range auto&& r) {
        std::cout << "Sorting range 3\n";
        std::sort(std::begin(r), std::end(r));
    }

    template<Range R>
    struct sorter {
        R& range;
        sorter(R& r) : range(r) {}
        void sort() {
            std::sort(std::begin(range), std::end(range));
        }
    };
    template<Range R> sorter(R&& r) -> sorter<R>;

    template<typename T>
    struct holder {
        T& value;
        holder(T& t) : value(t) {}
        void sort() requires(Range<T>) {
            std::sort(std::begin(value), std::end(value));
        }
    };
    template<typename T> holder(T&& t) -> holder<T>;

    void test() {
        std::cout << "============= example 3\n";
        std::vector<int> v{ 3, 2, 1 };

        static_assert(Range<std::vector<int>>);
        static_assert(!Range<int>);
        
        sort1(v);
        //sort1(0); // not a range
        sort2(v);
        sort3(v);

        int a[] { 3, 2, 1 };
        sort1(a);
        sort2(a);
        sort3(a);

        sorter rv(v);
        rv.sort();

        sorter ra(a);
        ra.sort();

        int i = 5;
        //sorter<int> fi(i); // constraint not satisfied

        holder hv(v);
        hv.sort();

        holder ha(a);
        //ha.sort(); // C arrays are hard to use in templates...
        // If you really need this to work, a partial specialization is much simpler than the alternative

        holder<int> hi(i);
        //hi.sort(); // constraint not satisfied
    }
}

namespace ex4 {
    namespace detail {
        template<typename T, typename = typename T::value_type> std::true_type test(int);
        template<typename T> std::false_type test(...); // failback
    }

    template<typename T>
    using has_value_type = decltype(detail::test<T>(0));
    template<typename T>
    using has_value_type_t = has_value_type<T>::type;
    template<typename T>
    inline constexpr bool has_value_type_v = has_value_type<T>::value;
    
    struct A {};

    void test() {
        std::cout << "============= example 4\n";
        static_assert(has_value_type<std::vector<int>>::value, "");
        static_assert(!has_value_type_v<A>, "");
        std::cout << has_value_type<std::vector<int>>::value << std::endl;
        std::cout << has_value_type<int>::value << std::endl;
    }
}

namespace ex5 {
    namespace detail {
        template<typename T, std::enable_if_t<sizeof(typename T::value_type) != 0, bool> = true> std::true_type test(int);
        template<typename T> std::false_type test(...); // failback
    }

    template<typename T>
    using has_value_type = decltype(detail::test<T>(0));
    template<typename T>
    using has_value_type_t = has_value_type<T>::type;
    template<typename T>
    inline constexpr bool has_value_type_v = has_value_type<T>::value;
    
    struct A {};

    void test() {
        std::cout << "============= example 5\n";
        
        static_assert(has_value_type<std::vector<int>>::value, "");
        static_assert(!has_value_type_v<A>, "");
        std::cout << has_value_type<std::vector<int>>::value << std::endl;
        std::cout << has_value_type<int>::value << std::endl;
    }
}

namespace ex6 {
    namespace detail {
        template<typename T> std::true_type test(int T::*); // selected if T is a class type
        template<typename T> std::false_type test(...);     // selected otherwise
    }

    template<typename T>
    using is_class = decltype(detail::test<T>(nullptr));
    template<typename T>
    using is_class_t = is_class<T>::type;
    template<typename T>
    inline constexpr bool is_class_v = is_class<T>::value;
    
    struct A {};

    union U {
        int i;
        char c;
    };

    void test() {
        std::cout << "============= example 6\n";
        static_assert(is_class<std::vector<int>>::value, "");
        static_assert(is_class_v<A>, "");
        static_assert(!is_class_v<int>, "");
        std::cout << is_class<std::vector<int>>::value << std::endl;
        std::cout << is_class<A>::value << std::endl;
        std::cout << is_class<int>::value << std::endl;

        /**
         * Unions are not considered classes by std::is_class, but implementing std::is_union requires compiler support, not SFINAE.
         */
        std::cout << is_class_v<U> << " " << std::is_class_v<U> << std::endl;
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
}