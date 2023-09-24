#pragma once

// If you want to use accum_trait_v2 before C++17, 
// it is good to implement inline member function for value trait
namespace accum_trait_v3
{

template<typename T>
struct AccumulationTraits;

template<>
struct AccumulationTraits<char> {
    using AccT = int;
    static constexpr AccT zero() {
        return 0;
    }
};
template<>
struct AccumulationTraits<short> {
    using AccT = int;
    static constexpr AccT zero() {
        return 0;
    }
};
template<>
struct AccumulationTraits<int> {
    using AccT = long;
    static constexpr AccT zero() {
        return 0l;
    }
};
template<>
struct AccumulationTraits<unsigned int> {
    using AccT = unsigned long;
    static constexpr AccT zero() {
        return 0lu;
    }
};
template<>
struct AccumulationTraits<float> {
    using AccT = double;
    static constexpr AccT zero() {
        return 0.;
    }
};

class BigInt {
public:
    BigInt(long long) {};
    // ...
};
template<>
struct AccumulationTraits<BigInt> {
    using AccT = BigInt;
    static AccT zero() {
        return BigInt{0ll};
    }
};

}