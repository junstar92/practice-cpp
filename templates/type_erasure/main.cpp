/** Bridging Static and Dynamic Polymorphism (Type Erasure)
 * 
 * C++ has static polymorphism (via template) and dynamic polymorphism (via ingeritance and virtual functions).
 * Both kinds of polymorphism provide powerful abstractions for writing programs, yet each has tradeoffs:
 * Static polymorphism provides the same performance as nonpolymorphic code, but the set of types that can be
 * used at run time is fixed at compile time. On the other hand, dynamic polymorphism via inheritance allows
 * a single version of the polymorphic function to work with types not known at the time it is compiled, but
 * it is less flexible because types must inherit from the common base classes.
 * 
 * This code describes how to bridge between static and dynamic polymorphism in C++, providing some of the
 * benefits from each model: the smaller executable code size and (alomos) entirely compiled nature of dynamic
 * polymorphism, along with the interface flexibility of static polymorphism that allows, for example, built-in
 * types to work seamlessly.
 * As an example, we will build a simplified version of the standard library's std::function<> template.
 */
#include <iostream>
#include <vector>
#include "function_ptr.h"

void printInt(int i)
{
    std::cout << i << ' ';
}

template<typename F>
void forUpTo1(int n, F f)
{
    for (int i = 0; i < n; ++i) {
        f(i); // call passed function f for i
    }
}

void forUpTo2(int n, void(*f)(int))
{
    for (int i = 0; i < n; ++i) {
        f(i); // call passed function f for i
    }
}

void forUpTo3(int n, std::function<void(int)> f)
{
    for (int i = 0; i < n; ++i) {
        f(i); // call passed function f for i
    }
}

void forUpTo4(int n, FunctionPtr<void(int)> f)
{
    for (int i = 0; i < n; ++i) {
        f(i); // call passed function f for i
    }
}

int main()
{
    {
        std::vector<int> values;

        // insert values from 0 to 4:
        forUpTo1(5, [&values](int i) {
            values.push_back(i);
        });

        // print elements:
        forUpTo1(5, printInt);
        std::cout << std::endl;
        /**
         * The forUpTo1() function template can be used with any function object, including lambda, function pointer,
         * or any class that either implements a suitable operator() or a conversion to a function pointer or reference,
         * and each use of forUpTo1() will likely produce a difference instantiation of the function template. This 
         * function template is fairly small, but if the template were large, it is possible that these instantiations
         * could increase code size.
         */
    }
    std::cout << "----------------------------\n";
    {
        /**
         * One approach to limit this increase in code size is to turn the function template into a nontemplate, which
         * needs no instantiation. For example, we might attempt to do this with a function pointer as forUpTo2.
         * However, while this implementation will work when passed printInt(), it will produce an error when passed
         * the lambda.
         */
        std::vector<int> values;

        // insert values from 0 to 4: -> ERROR: lambda not convertible to a function pointer
        // forUpTo2(5, [&values](int i) {
        //     values.push_back(i);
        // });

        values = {0, 1, 2, 3, 4};
        // print elements: -> OK: print 0 1 2 3 4
        forUpTo2(5, printInt);
        std::cout << std::endl;
    }
    std::cout << "----------------------------\n";
    {
        /**
         * The standard library's class template std::function<> permits an alternative formulation: forUpTo3().
         * The template argument to std::function<> is a function type that desecribe the parameter types the function 
         * object will receive and the return type that it should produce, much like a function pointer describes the
         * parameter and result types.
         * 
         * This formulation of forUpTo3() provides some aspects of static polymorphism - the ability to work with an
         * unbounded set of types including function pointers, lambdas, and arbitrary classes with a suitable operator()
         * - while itself remaining a nontemplate function with a single implementation. It does so using technique called
         * `type erasure`, which bridges the gap between static and dynamic polymorphism.
         */
        std::vector<int> values;

        // insert values from 0 to 4:
        forUpTo3(5, [&values](int i) {
            values.push_back(i);
        });

        // print elements:
        forUpTo3(5, printInt);
        std::cout << std::endl;
    }
    std::cout << "----------------------------\n";
    {
        /**
         * The std::function<> type is effectively a generatlized form of a C++ function pointer, providing the same
         * fundamental operations:
         * - It can be used to invoke a function without the caller knowing anything about the function itself.
         * - It can be copied, moved, and assigned.
         * - It can be initialized or assigned from another function (with a compatible signature).
         * - It has a 'null' state that indicates when no function is bound to it.
         * However, unlike a C++ function pointer, a std::function<> can also store a lambda or any other function object
         * with a suitable operator(), all of which may have different types.
         * 
         * We build our own generatlized function pointer class template, FunctionPtr.
         * Refer to "function_ptr.h" file for more details.
         */
        std::vector<int> values;

        // insert values from 0 to 4:
        forUpTo4(5, [&values](int i) {
            values.push_back(i);
        });

        // print elements:
        forUpTo4(5, printInt);
        std::cout << std::endl;
    }
    /** Performance Considerations when Using Type Erasure
     * Type erasure provides some of the advantages of both static polymorphism and dynamic polymorphism, but not all.
     * In particular, the performance of generated code using type erause hews more closely to that of dynamic polymorphism,
     * because both use dynamic dispatch via virtual functions. Thus, some of the traditional advantages of static polymorphism,
     * such as the ability of the compiler to inline calls, may be lost. Whether this loss of performance will be preceptible is
     * application-dependent, but it's often easy to tell by considering how much work is performed in the function being
     * called relative to the cost of a virtual function call: If the two are close (e.g., using FunctionPtr to simply add two
     * integers), type erasure is likely to execute far more slowly than a static-polymorphic version. If, on the other hand,
     * the function call performs a significant amount of work - querying a database, sorting a container, or updating a user
     * interface - the overhead of type erasure is unlikely to be measurable.
     */
}