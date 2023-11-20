#include <iostream>
#include <string>
#include <type_traits>
#include <array>

namespace v1 {

// it needs to end the recursion
void print()
{
}

// using template parameter pack = '...'
template<typename T, typename... Types>
void print(T first_arg, Types... args)
{
    std::cout << first_arg << std::endl; // print first argument
    print(args...); // call print() for remaining arguments
}

}

namespace v2 {
    
template<typename T>
void print(T arg)
{
    std::cout << arg << std::endl;
}

template<typename T, typename... Types>
void print(T first_arg, Types... args)
{
    print(first_arg); // call print() for the first argument
    print(args...); // call print() for remaining
}

}

namespace v3 {

// Operator 'sizeof...'
template<typename T, typename... Types>
void print_sizeof(T first_arg, Types... args)
{
    std::cout << sizeof...(Types) << std::endl; // print number of remaining types
    std::cout << sizeof...(args) << std::endl; // print number of remaining args
    // ...
}

template<typename T, typename... Types>
void print(T first_arg, Types... args)
{
    std::cout << first_arg << std::endl;
    if constexpr (sizeof...(args) > 0) { // since C++17, compile-time if is available
        print(args...);                  // so, if there are no more arguments, this condition and body are deleted.
    }
}

}

namespace foldexp {

template<typename... T>
auto fold_sum(T... s) {
    return (... + s); // return ((s1 + s2) + s3) ...
}

template<typename... Types>
void print_v1(Types const&... args)
{
    (std::cout << ... << args) << std::endl;
}

template<typename T>
class AddSpace
{
private:
    T const& ref;

public:
    AddSpace(T const& r) : ref{r} {}
    friend std::ostream& operator<<(std::ostream& os, AddSpace<T> s) {
        return os << s.ref << ' '; // output passed argument ad a space
    }
};

template<typename... Args>
void print_v2(Args... args)
{
    (std::cout << ... << AddSpace(args)) << std::endl;
}

}

namespace varexp {

template<typename... T>
void print_doubled(T const&... args)
{
    v3::print(args + args...);
}

template<typename... T>
void add_one(T const&... args)
{
    // v3::print(args + 1...); // error: 1... is a literal with too many demical points
    v3::print(args + 1 ...); // OK
    std::cout << std::endl;
    v3::print((args + 1)...); // OK
}

// compile-time expression can include template parameter packs
template<typename T1, typename... TN>
constexpr bool is_homogeneous(T1, TN...)
{
    return (std::is_same<T1, TN>::value&& ...);
}

}

namespace varidx {

template<typename C, typename... Idx>
void print_elems(C const& coll, Idx... idx)
{
    v3::print(coll[idx]...);
}

template<std::size_t... Idx, typename C>
void print_idx(C const& coll)
{
    v3::print(coll[Idx]...);
}

}

namespace varcls {

// type for arbitrary number of indices
template<std::size_t...>
struct Indices {};

template<typename T, std::size_t... Idx>
void print_by_idx(T t, Indices<Idx...>)
{
    v3::print(std::get<Idx>(t)...);
}

}

int main()
{
    // print() for a variable number of arguments of different types (version 1)
    std::string s{"world"};
    v1::print(7.5, "hello", s);
    std::cout << std::endl;

    // print() using variadic templates  (version 2)
    v2::print(7.5, "hello", s);
    std::cout << std::endl;

    /*** sizeof... ***/
    // print number of remaining types and args
    v3::print_sizeof(7.5, "hello", s);
    std::cout << std::endl;

    // print() version 3 that can skip the function for the end of the recursion
    // by not calling it in case there are no more arguments
    v3::print(7.5, "hello", s);
    std::cout << std::endl;

    /*** Fold Expression ***/
    std::cout << "fold_sum(1, 2, 3, 4, 5) = " << foldexp::fold_sum(1, 2, 3, 4, 5) << std::endl;

    foldexp::print_v1(7.5, "hello", s); // in this case, no whitespace separates all the elements from the parameter pack.
    std::cout << std::endl;

    foldexp::print_v2(7.5, "hello", s); // add whitespace between argument outputs
    std::cout << std::endl;


    /*** Variadic Expressions ***/
    varexp::print_doubled(7.5, std::string("hello"), s);
    std::cout << std::endl;
    // the function has the following effect
    // print(7.5 + 7.5, std::string("hello") + std::string("hello"), s + s);

    varexp::add_one(1, 2, 3, 4, 5);
    std::cout << std::endl;

    std::cout << std::boolalpha << varexp::is_homogeneous(1, 2, 3.f, "abc") << std::endl;
    std::cout << std::boolalpha << varexp::is_homogeneous("hello", "", "world", "1") << std::endl;
    std::cout << std::endl;


    /*** Variadic Indices ***/
    std::vector<std::string> coll = {"good", "times", "say", "bye"};
    varidx::print_elems(coll, 2, 0, 3);
    std::cout << std::endl;

    varidx::print_idx<2, 0, 3>(coll);
    std::cout << std::endl;

    
    /*** Variadic Class Template ***/
    std::array<std::string, 5> arr = {"Hello", "my", "new", "!", "World"};
    varcls::print_by_idx(arr, varcls::Indices<0, 4, 3>());
}