#include <iostream>
#include "typelist.h"
#include "valuelist.h"

using SignedIntegralTypes = Typelist<signed char, short, int, long, long long>;

template<typename T, typename U>
struct SmallerThanT {
    static constexpr bool value = sizeof(T) < sizeof(U);
};

template<typename T>
struct AddConstT
{
    using Type = T const;
};
template<typename T>
using AddConst = typename AddConstT<T>::Type;

void test_insertion_sort() {
    using Types = Typelist<int, char, short, double>;
    using ST = InsertionSort<Types, SmallerThanT>;
    std::cout << "std::is_same_v<ST, Types> = " << std::is_same_v<ST, Typelist<char, short, int, double>> << std::endl;
}

using Primes = Typelist<CTValue<int, 2>, CTValue<int, 3>, CTValue<int, 5>,
                        CTValue<int, 7>, CTValue<int, 11>>;
using Primes2 = CTTypelist<int, 2, 3, 5, 7, 11>;

template<typename T, typename U>
struct MultiplyT;

template<typename T, T Value1, T Value2>
struct MultiplyT<CTValue<T, Value1>, CTValue<T, Value2>> {
    using Type = CTValue<T, Value1 * Value2>;
};
template<typename T, typename U>
using Multiply = typename MultiplyT<T, U>::Type;

template<typename T, typename U>
struct GreaterThanT;
template<typename T, T First, T Second>
struct GreaterThanT<CTValue<T, First>, CTValue<T, Second>> {
    static constexpr bool value = First > Second;
};

void test_valuelist() {
    using Integers = Valuelist<int, 6, 2, 4, 9, 5, 2, 1, 7>;
    using SortedIntegers = InsertionSort<Integers, GreaterThanT>;

    std::cout << "std::is_same_v<SortedIntegers, Valuelist<int, 9, 7, 6, 5, 4, 2, 2, 1>> = "
        << std::is_same_v<SortedIntegers, Valuelist<int, 9, 7, 6, 5, 4, 2, 2, 1>> << std::endl;  
}

int main()
{
    // Typelist Test
    PopFront<SignedIntegralTypes> a1; // produces the typelist Typelist<short, int, long, long long>
    NthElement<Typelist<short, int, long>, 2> a2;
    LargestType<SignedIntegralTypes> a3;
    PushBack<Typelist<short, int>, long> a4; // produces the typelist Typelist<short, int, long>
    Reverse<Typelist<short, int, long>> a5; // produces the typelist Typelist<long, int, short>

    PopBack<Typelist<short, int, long>> a6; // produces the typelist Typelist<short, int>
    std::cout << "std::is_same_v<decltype(a6), Typelist<short, int>> = " << std::boolalpha << std::is_same_v<decltype(a6), Typelist<short, int>> << std::endl;

    Transform<SignedIntegralTypes, AddConstT> a7; // produces the typelist Typelist<const signed char, const short, const int, const long, const long long>
    std::cout << "std::is_same_v<decltype(a7), Typelist<const signed char, const short, const int, const long, const long long> = "
        << std::is_same_v<decltype(a7), Typelist<const signed char, const short, const int, const long, const long long>> << std::endl;

    using Result = Accumulate<SignedIntegralTypes, PushFrontT, Typelist<>>; // produces Typelist<long long, long, int, short, signed char>
    std::cout << "std::is_same_v<Result, Reverse<SignedIntegralTypes>> = " << std::is_same_v<Result, Reverse<SignedIntegralTypes>> << std::endl;

    test_insertion_sort();

    // Nontype Typelist Test
    std::cout << "2 x 3 x 5 x 7 x 11 = " << Accumulate<Primes, MultiplyT, CTValue<int, 1>>::value << std::endl;
    
    test_valuelist();

    using ReversedSignedIntegralTypes = Select<SignedIntegralTypes, Valuelist<unsigned, 4, 3, 2, 1, 0>>;
    std::cout << "std::is_same_v<ReversedSignedIntegralTypes, Reverse<SignedIntegralTypes>> = "
        << std::is_same_v<ReversedSignedIntegralTypes, Reverse<SignedIntegralTypes>> << std::endl;

    return 0;
}