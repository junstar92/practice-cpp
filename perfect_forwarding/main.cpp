/**
 * Suppose you want to write generic code that forwards the basic property of passed arguments:
 * - Modifyable objects should be forwarded so that tehy still can be modified.
 * - Constant objects should be forwarded as read-only objects.
 * - Movable objects (objects we can "steal" from because they are about to expire) should be forwared as movable objects.
 * To achieve this functionality without templates, we have to program all three cases.
 * 
 * For example, to forward a call of f() to a corresponding function g():
 */
#include <iostream>
#include <utility>

class X {};

void g(X&) {
    std::cout << "g() for variable\n";
}
void g(X const&) {
    std::cout << "g() for constant\n";
}
void g(X&&) {
    std::cout << "g() for movable object\n";
}

// let f() forward argument val to g():
void f(X& val) {
    g(val);             // val is non-const lvalue => calls g(X&)
}
void f(X const& val) {
    g(val);             // val is const lvalue => calls g(X const&)
}
/**
 * Note that the code for movable objects (via an rvalue reference) differs from the other code: It needs a
 * std::move() because according to language rules, move semantics is not passed through. Although val is the
 * f(X&& val) is declared as rvalue reference its value category when used as expression is a nonconstant lvalue
 * and behaves as val is the first f(X& val). Without the std::move(), g(X&) for non-constant lavlues instead of
 * g(X&&) would be called.
 */
void f(X&& val) {
    g(std::move(val));  // val is non-const lvalue => needs std::move to call g(X&&)
}

/**
 * If we want to combine all three cases of f() in generic code, we have a problem:
 */
template<typename T>
void f1(T val) {
    g(val);
}
/**
 * This code works for the first two cases, but not for the third case where movable objects are passed.
 * C++11 for this reason introduces special rules for perfect forwarding parameters. This idiomatic code pattern
 * to achieve this is as follows:
 */
template<typename T>
void f2(T&& val) {
    g(std::forward<T>(val)); // perfect forward val to g()
                             // call the right g() for any passed argument val
}
/**
 * Note that std::move() has no template parameter and "triggers" move semantics for the passed argument, while
 * std::forward<>() "forwards" potential move semantic depending on a passed template argument.
 * 
 * Don's assume that T&& for at template paramter T behaves as X&& for a specific type X. Different rules apply:
 * - X&& for a specific type X declares a parameter to be an rvalue reference. It can only be bound to a movable
 *   object (a prvalue, such as a temporary object, and an xvalue, such as an object passed with std::move()). 
 *   It is always mutable and you can always "steal" its value.
 * - T&& for a template parameter T decalres a forwarding reference (also called universal reference). It can be
 *   bound to a mutable, immutable(i.e., const), or movable object. Inside the function definition, the parameter
 *   may be mutable, immutable, or refer to a value you can "steal" the internals from.
 * 
 * Note that T must really be the name of a template parameter. Depending on a template parameter is not sufficient.
 * For a template parameter T, a declaration such as typename T::iterator&& is just an rvalue reference, not a
 * forwarding reference.
 */


int main()
{
    X v;
    X const c;
    std::cout << "----------------------------\n";
    f(v);               // f() for non-constant object calls f(X&) => calls g(X&)
    f(c);               // f() for constant object calls f(X const&) => calls g(X const&)
    f(X());             // f() for temporary calls f(X&&) => calls g(X&&)
    f(std::move(v));    // f() for movable variable calls f(X&&) => calls g(X&&)

    std::cout << "----------------------------\n";
    f1(v);               // f1() for non-constant object calls f1(X&) => calls g(X&)
    f1(c);               // f1() for constant object calls f1(X&) => calls g(X&)
    f1(X());             // f1() for temporary calls f1(X&) => calls g(X&)
    f1(std::move(v));    // f1() for movable variable calls f1(X&) => calls g(X&)

    std::cout << "----------------------------\n";
    f2(v);               // f2() for non-constant object calls f2(X&) => calls g(X&)
    f2(c);               // f2() for constant object calls f2(X const&) => calls g(X const&)
    f2(X());             // f2() for temporary calls f2(X&&) => calls g(X&&)
    f2(std::move(v));    // f2() for movable variable calls f2(X&&) => calls g(X&&)
}