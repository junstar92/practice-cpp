#pragma once

// add value traits for initial zero value
namespace accum_trait_v2
{

template<typename T>
struct AccumulationTraits;

template<>
struct AccumulationTraits<char> {
    using AccT = int;
    static AccT constexpr zero = 0;
};
template<>
struct AccumulationTraits<short> {
    using AccT = int;
    static AccT constexpr zero = 0;
};
template<>
struct AccumulationTraits<int> {
    using AccT = long;
    static AccT constexpr zero = 0l;
};
template<>
struct AccumulationTraits<unsigned int> {
    using AccT = unsigned long;
    static AccT constexpr zero = 0lu;
};
template<>
struct AccumulationTraits<float> {
    using AccT = double;
    // 'const' static member cannot be initialized by floating point type, 
    // so, must use 'constexpr'.
    static AccT constexpr zero = 0.;
};

// but the types which are not literal cannot be initialized by using both 'const' and 'constexpr', e.g., user defined type.
// So, the code below occurs error.
class BigInt {
public:
    BigInt(long long) {};
    // ...
};
/*
template<>
struct AccumulationTraits<BigInt> {
    using AccT = BigInt;
    static BigInt constexpr zero = BigInt{0ll}; // ERROR: not a literal type
};
*/
// before C++17, it is possible to declare value trait in 'AccumulationTraits' but not to define it.
// and add the initialization for value trait in source code.
/*
template<>
struct AccumulationTraits<BigInt> {
    using AccT = BigInt;
    static BigInt const zero; // only declaration
};
// definition in cpp source file
BigInt const AccumulationTraits<BigInt>::zero = BigInt{0ll};
*/
// since C++17, it is resolved simply by using 'inline' static member variable.
template<>
struct AccumulationTraits<BigInt> {
    using AccT = BigInt;
    inline static BigInt const zero = BigInt{0ll}; // OK since C++17
};

}