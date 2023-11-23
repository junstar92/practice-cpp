/** Transformation Traits
 * Traits can perform transformation on types, such as adding or removing references or const
 * and volatile qualifiers.
 */

/** Removing References
 * For example, we can implement a RemoveReferenceT trait that turns reference types into their
 * underlying object or function types, and leaves nonreference types alone.
 * 
 * Removing the reference from a type is typically useful when the type was derived using a
 * construct that sometimes produces reference types, such as the special deduction rule for
 * function parameters of type T&&.
 * The C++ standard library provides a corresponding type trait std::remove_reference<>.
 */
template<typename T>
struct RemoveReferenceT {
    using Type = T;
};
template<typename T>
struct RemoveReferenceT<T&> {
    using Type = T;
};
template<typename T>
struct RemoveReferenceT<T&&> {
    using Type = T;
};
// A convenience alias template makes the usage simpler:
template<typename T>
using RemoveReference = typename RemoveReferenceT<T>::Type;

/** Adding Reference
 * Similarly, we can take an existing type and create an lvalue or rvalue reference from it (along
 * with the usual convenience alias templates).
 * 
 * The rules of reference collapsing apply here. For example, calling AddLValueReference<int&&> produces
 * type int& (there is therefore no need to implement them manually via partial specialization).
 * 
 * The C++ standard library provides corresponding type traits std::add_lvalue_reference<> and
 * std::add_rvalue_reference<>. The standard templates include the specialization for void types.
 */
template<typename T>
struct AddLValueReferenceT {
    using Type = T&;
};
template<typename T>
using AddLValueReference = typename AddLValueReferenceT<T>::Type;
template<typename T>
struct AddRValueReferenceT {
    using Type = T&&;
};
template<typename T>
using AddRValueReference = typename AddRValueReferenceT<T>::Type;

/**
 * If we leave AddLValueReferenceT and AddRValueReferenceT as they are and do not introduce specializations
 * of them, then the convenience aliases can actually be simplified to below. It can be instantiated without
 * instantiating a class template (and is therefore a lighter-weight process). However, this is risky, as we
 * may well want to specialize these template for special cases. For example, we cannot use void as a template
 * argument for these templates.
 */
// template<typename T>
// using AddLValueReferenceT = T&;
// template<typename T>
// using AddRValueReferenceT = T&&;

// A few explicit specializations can take care of void type:
template<>
struct AddLValueReferenceT<void> {
    using Type = void;
};
template<>
struct AddLValueReferenceT<void const> {
    using Type = void const;
};
template<>
struct AddLValueReferenceT<void volatile> {
    using Type = void volatile;
};
template<>
struct AddLValueReferenceT<void const volatile> {
    using Type = void const volatile;
};
template<>
struct AddRValueReferenceT<void> {
    using Type = void;
};
template<>
struct AddRValueReferenceT<void const> {
    using Type = void const;
};
template<>
struct AddRValueReferenceT<void volatile> {
    using Type = void volatile;
};
template<>
struct AddRValueReferenceT<void const volatile> {
    using Type = void const volatile;
};

/** Removing Qualifiers
 * Transformation traits can break down or introduce any kind of compound type, not just references.
 * For example, we can remove a const qualifier if present.
 */
template<typename T>
struct RemoveConstT {
    using Type = T;
};
template<typename T>
struct RemoveConstT<T const> {
    using Type = T;
};
template<typename T>
using RemoveConst = typename RemoveConstT<T>::Type;

// +) remove volatile traits
template<typename T>
struct RemoveVolatileT {
  using Type = T;
};
template<typename T>
struct RemoveVolatileT<volatile T> {
  using Type = T;
};
template<typename T>
using RemoveVolatile = typename RemoveVolatileT<T>::Type;

// Moreover, transformation traits can be composed, such as creating a RemoveCVT trait that removes both const
// and volatile
template<typename T>
struct RemoveCVT : RemoveConstT<typename RemoveVolatileT<T>::Type> {
};
template<typename T>
using RemoveCV = typename RemoveCVT<T>::Type;

/** Decay
 * To round out our discussion of transformation traits, we develop a trait that mimics type conversions when
 * passing arguments to parameters by value. Derived from C, this means that the arguments decay (turning array
 * types into pointers and function types into pointer-to-function types) and delete any top-level const, volitile,
 * or reference qualifiers (becuase top-level type qualifiers on parameter types are ignored when resolving a function
 * call).
 * The effect of this pass-by-value can be seen in the following program, which prints the actual parameter type produced
 * after the compiler decays the specified type.
 */
#include <iostream>
#include <typeinfo>
#include <type_traits>

template<typename T>
void f(T) {}

template<typename A>
void print_params_type(void(*)(A)) {
    std::cout << "Parameter type " << typeid(A).name() << "\n";
    std::cout << "- is int:      " << std::is_same<A, int>::value << "\n";
    std::cout << "- is const:    " << std::is_const<A>::value << "\n";
    std::cout << "- is pointer:  " << std::is_pointer<A>::value << "\n";
}

/**
 * We can implement a trait that produces the same type conversion of passing by value. To match to the C++ standard
 * library train std::deacy, we name it DecayT. Its implementation combines several of the techinques described above.
 * First, we define the nonarray, nonfunction case, which simply deletes any const and voltaile qualifiers.
 * Next, we handle the array-to-pointer decay, which requires us to recognize any array type (with or without a bound)
 * using partial specialization.
 * Finally, we handle the funciton-to-pointer decay, which has to match any function type, regardless of the return
 * type or the number of parameter types.
 */
template<typename T>
struct DecayT : RemoveCVT<T> {};

template<typename T>
struct DecayT<T[]> {
    using Type = T*;
};
template<typename T, std::size_t N>
struct DecayT<T[N]> {
    using Type = T*;
};

template<typename R, typename... Args>
struct DecayT<R(Args...)> {
    using Type = R(*)(Args...);
};
// Note that this partial specialization matches any function type that uses C-style varargs.
template<typename R, typename... Args>
struct DecayT<R(Args..., ...)> {
    using Type = R(*)(Args..., ...);
};

template<typename T>
void print_params_type() {
    using A = typename DecayT<T>::Type;
    std::cout << "Parameter type " << typeid(A).name() << "\n";
    std::cout << "- is int:      " << std::is_same<A, int>::value << "\n";
    std::cout << "- is const:    " << std::is_const<A>::value << "\n";
    std::cout << "- is pointer:  " << std::is_pointer<A>::value << "\n";
}

int main()
{
    std::cout << "----------------------------\n";
    {
        print_params_type(&f<int>);         // the int parameter has been left unchanged
        print_params_type(&f<int const>);   // the int const parameter has decayed to int - void(*)A => int(int)
        print_params_type(&f<int[7]>);      // the int[7] parameter has decayed to int*   - void(*)A => int(*)(int)
    }
    std::cout << "----------------------------\n";
    {
        print_params_type<int>();
        print_params_type<int const>();
        print_params_type<int[7]>();
        print_params_type<int(int)>();
    }
}