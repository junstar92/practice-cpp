#pragma once
#include <utility>
#include <type_traits>
#include <exception>

template<typename T>
class IsEqualityComparable
{
private:
    static void* conv(bool); // to check convertibility to bool
    template<typename U>
    static std::true_type test(decltype(conv(std::declval<U const&>() == std::declval<U const&>())),
                               decltype(conv(!(std::declval<U const&>() == std::declval<U const&>())))
                              );
    // fallback
    template<typename U>
    static std::false_type test(...);
public:
    static constexpr bool value = decltype(test<T>(nullptr, nullptr))::value;
};

template<typename T,
         bool EqComparable = IsEqualityComparable<T>::value>
struct TryEquals
{
    static bool equals(T const& x1, T const& x2) {
        return x1 == x2;
    }
};

class NotEqualityComparable : public std::exception {};

template<typename T>
struct TryEquals<T, false>
{
    static bool equals(T const& x1, T const& x2) {
        throw NotEqualityComparable();
    }
};