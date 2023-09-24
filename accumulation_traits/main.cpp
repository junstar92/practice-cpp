#include <iostream>
#include <accum_traits_v1.h>
#include <accum_traits_v2.h>
#include <accum_traits_v3.h>
#include <iterator>

template<typename T>
T accum0(T const* beg, T const* end)
{
    T total{}; // assume this actually creates a zero value
    while (beg != end) {
        total += *beg;
        ++beg;
    }
    return total;
}

// fixed ver 1
template<typename T>
auto accum1(T const* beg, T const* end)
{
    // return type is traits of the element type
    using AccT = typename accum_trait_v1::AccumulationTraits<T>::AccT;

    AccT total{}; // still assume this actually creates a zero value
    while (beg != end) {
        total += *beg;
        ++beg;
    }
    return total;
}

// fixed ver 2
template<typename T>
auto accum2(T const* beg, T const* end)
{
    // return type is traits of the element type
    using AccT = typename accum_trait_v2::AccumulationTraits<T>::AccT;

    AccT total = accum_trait_v2::AccumulationTraits<T>::zero; // initialize total by trait value
    while (beg != end) {
        total += *beg;
        ++beg;
    }
    return total;
}

// fixed ver 3
template<typename T>
auto accum3(T const* beg, T const* end)
{
    // return type is traits of the element type
    using AccT = typename accum_trait_v3::AccumulationTraits<T>::AccT;

    AccT total = accum_trait_v3::AccumulationTraits<T>::zero(); // using static member function
    while (beg != end) {
        total += *beg;
        ++beg;
    }
    return total;
}

// fixed ver 4
// All previous implementations are called fixed traits.
// A implementation is called 'parameterized trait'.
template<typename T, typename AT = accum_trait_v3::AccumulationTraits<T>>
auto accum4(T const* beg, T const* end)
{
    typename AT::AccT total = AT::zero();
    while (beg != end) {
        total += *beg;
        ++beg;
    }
    return total;
}

// fixed ver 5: using general iterators - std::iterator_traits in <iterator> header
template<typename Iter>
auto accum5(Iter start, Iter end)
{
    using VT = typename std::iterator_traits<Iter>::value_type;

    VT total{}; // assume this actually creates a zero value
    while (start != end) {
        total += *start;
        ++start;
    }
    return total;
}

int main(int argc, char** argv)
{
    // create array of 5 integer value
    int num[] = { 1, 2, 3, 4, 5 };
    
    // print average value - expected value is 3
    std::cout << "the average value of the integer values is " << accum0(num, num+5) / 5 << std::endl;

    // create array of character values and try to print average character value
    char name[] = "templates";
    int length = sizeof(name) - 1;
    std::cout << "the average value of the characters in \"" << name << "\" is "
        << accum0(name, name + length) / length << std::endl;
    // expected value will be between 'a'(97) and 'z'(122), but is -5.
    // because accum0 template is instantiated for 'char' type, value range is narrow.

    std::cout << "\n========== fixed ver 1 ==========\n";
    std::cout << "the average value of the characters in \"" << name << "\" is "
        << accum1(name, name + length) / length << std::endl;
    
    // the problem is resolved, but there is a potential problem for zero (initial value)
    std::cout << "\n========== fixed ver 2 ==========\n";
    std::cout << "the average value of the characters in \"" << name << "\" is "
        << accum2(name, name + length) / length << std::endl;

    std::cout << "\n========== fixed ver 3 ==========\n";
    std::cout << "the average value of the characters in \"" << name << "\" is "
        << accum3(name, name + length) / length << std::endl;

    std::cout << "\n========== fixed ver 4 ==========\n";
    std::cout << "the average value of the characters in \"" << name << "\" is "
        << accum4(name, name + length) / length << std::endl;

    std::cout << "\n========== fixed ver 5 ==========\n";
    // result value is -5 because value type is char.
    std::cout << "the average value of the characters in \"" << name << "\" is "
        << accum5(name, name + length) / length << std::endl;


    return 0;
}