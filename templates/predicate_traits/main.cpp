/** Predicate Traits **/


/** IsSameT
 * The IsSameT trait yields whether two types are equal
 * Here the primary template defines that, in general, two different types passed as template arguments differ
 * so that the value member is false. However, using partial specialization, when we have the special case that
 * the two passed types are the same, value is true.
 * 
 * The C++ standard library provides a corresponding type trait std::is_same<>.
 */
namespace v1 {

template<typename T1, typename T2>
struct IsSameT {
    static constexpr bool value = false;
};
template<typename T>
struct IsSameT<T, T> {
    static constexpr bool value = true;
};
template<typename T1, typename T2>
constexpr bool isSame = IsSameT<T1, T2>::value;

}
/** true_type and false_type
 * We can significantly improve the definition of IsSameT by providing different types for the possible two
 * outcomes, true and false. In fact, if we declare a class template BoolConstant with the two possible instatiations
 * TrueType and FalseType.
 * 
 * Fortunately, the C++ standard library provides corresponding types in <type_traits> since C++11: 
 * std::true_type and std::false_type.
 */
namespace v2 {

template<bool val>
struct BoolConstant {
    using Type = BoolConstant<val>;
    static constexpr bool value = val;
};
using TrueType = BoolConstant<true>;
using FalseType = BoolConstant<false>;

// We can define IsSameT so that, depending on whether the two types match, it derives from TrueType or FalseType
template<typename T1, typename T2>
struct IsSameT : FalseType {};
template<typename T>
struct IsSameT<T, T> : TrueType {};

}

#include <iostream>

template<typename T>
void foo_impl(T, v2::TrueType) {
    std::cout << "foo_impl(T, true) for int called\n";
}

template<typename T>
void foo_impl(T, v2::FalseType) {
    std::cout << "foo_impl(T, false) for other type called\n";
}

template<typename T>
void foo(T t) {
    foo_impl(t, v2::IsSameT<T, int>{}); //choose impl depending on whether T is int
}

int main()
{
    std::cout << "----------------------------\n";
    foo(42);    // calls foo_impl(42, TrueType)
    foo(7.7);   // calls foo_impl(7.7, FalseType)
}