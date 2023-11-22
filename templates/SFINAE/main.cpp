/** SFINAE (Subsitution Failure Is Not An Error)
 * In C++, it is pretty common to overload functions to account for various argument types.
 * When a compiler sees a call to an overloaded function, it must therefore consider each
 * candidate separately, evaluating the arguments of the call and picking the candidate that
 * matches best.
 * In cases where the set of cadidates for a call includes function templates, the compiler
 * first has to determine what template arguments should be used for that candidate, then
 * substitute those arguments in the function parameter list and in its return type, and then
 * evaluate how well it matches (just like an ordinary function). However, the subsitution
 * process could run into problems: It could produce constructs that make no sense. Rather
 * than deciding that such meaningless substitutions lead to errors, the lanugage rules instead
 * say that candidates with such substitution problems are simply ignored.
 * We call this principle SFINAE (pronounced like sfee-nay), which stands for "substitution
 * failure is not an error.".
 * 
 * - SFINAE and Overload Resolution
 * Over time, the SFINAE principle has become so important and so prevalent among template
 * designers that the abbreviation has become a verb. We say "we SFINAE out a function" if we
 * mean to apply the SFINAE mechanism to ensure that function templates are ignored for certain
 * constrains by instrumenting the template code to result in invalid code for the these constraints.
 * And whenever you read in the C++ standard that a function template "shall not participlate in 
 * overload resolution unless.." it means that SFINAE is used to "SFINAE out" that function template
 * for certain cases.
 * For example, there is a class `std::thread`'s constructor.
 * : Remarks: This constructor shall not participate in overload resolution if decay_t<F> is the same
 * type as std::thread.
 * This means that the template constructor is ignored if it is called with a std::thread as first and
 * only argument. The reason is that otherwise a member template like this sometimes might better match
 * than any predefined copy or move constructor. By SFINAE'ing out the constructor template when called
 * for a thread, we ensure that the predefined copy or move constructor is always used when a thread gets
 * contructed from another thread.
 */
#include <iostream>
#include <vector>

namespace len1 {

/**
 * Here, we define two function templates len() taking one gerneric argument:
 * 1. The first function template declares the parameter as T(&)[N], which means that the parameter
 * has to be an array of N elements of type T.
 * 2. The second function template declares the parameter simply as T, which places no constraints
 * on the parameter but returns type T::size_type, which requires that the passed argument type has
 * a corresponding member size_type.
 */

// number of elements in a raw array:
template<typename T, unsigned N>
std::size_t len(T(&)[N]) {
    return N;
}

// number of elements for a type having size_type"
template<typename T>
typename T::size_type len(T const& t) {
    return t.size();
}

}

namespace len2 {

// number of elements in a raw array:
template<typename T, unsigned N>
std::size_t len(T(&)[N]) {
    return N;
}

// number of elements for a type having size_type"
template<typename T>
typename T::size_type len(T const& t) {
    return t.size();
}

// fallback for all other types:
std::size_t len(...) {
    return 0;
}

}

/** Expression SFINAE with decltype
 * it is not alwasy easy to find out and formulate the right expression to SFINAE out function templates for
 * certain conditions.
 * Suppose, for example, that we want to ensure that the function template len() is ignored for arguments of
 * a type that has a size_type member but not a size() member function as demonstrated above. Without any form
 * of requirements for a size() member function in the function declaration, the function template is selected
 * and its ultimate instantiation then results in an error:
 * ```
 * template<typename T>
 *   typename T::size_type len(T const& t) {
 *   return t.size();
 * }
 * 
 * std::allocator<int> x;
 * std::cout << len(x) << "\n"; // ERROR: len() selected, but x has no size()
 * ```
 * There is a common pattern or idiom to deal with such a situation:
 * - Specify the return type with the trailing return type syntax (use auto at the front and -> before the return
 *   type at the end).
 * - Define the return type using decltype and the comma operator.
 * - Formulate all expressions that must be valid at the beginning of the comma operator (converted to void in case
 *   the comma operator is overloaded).
 * - Define an object of the real return type at the end of the comma operator.
 * For example, refer to namespace len3.
 */
namespace len3 {

/**
 * In this function, the return type is given by `decltype( (void)(t.size)(), T::size_type() )`.
 * The operand of the decltype construct is a comma-separated list of expressions, so that the last expression
 * T::size_type() yields a value of the desired return type (which decltype uses to convert into the return type).
 * Before the (last) comma, we have the expressions that must be valid, which in this case is just t.size(). The
 * case of the expression to void is to avoid the possibility of a user-defined comma operator overloaded for the
 * type of the expression.
 * 
 * Note that the argument of decltype is an unevaluated operand, which means that you, for example, can create 
 * "dummy objects" without calling constructors.
 */
template<typename T>
auto len(T const& t) -> decltype( (void)(t.size()), T::size_type() ) {
    return t.size();
}

}

int main()
{
    std::cout << "----------------------------\n";
    {
        int a[10];
        std::cout << len1::len(a) << std::endl;     // OK: only len1::len() for array matches
        std::cout << len1::len("tmp") << std::endl; // OK: only len1::len() for array matches

        std::vector<int> v;
        std::cout << len1::len(v) << std::endl;     // OK: only len1::len() for a type with size_type matches

        /**
         * When passing a raw pointer, neither of the templates match (without a failure). As a result,
         * compiler will complain that no matching len1::len() function is found.
         */
        int* p;
        // std::cout << len1::len(p) << std::endl;     // ERROR: no matching len1::len() function found

        /**
         * Note that this above differs from passing an object of a type having a size_type member, but no size()
         * member function, as is, for example, the case for std::allocator<>. When passing an object of such a
         * type, the compiler finds the second function template in namespace len1 as matching function template.
         * So instead of an error that no matching len1::len() function is found, this will result in a compile-time
         * error that calling size() for a std::allocator<int> is invalid. This time, the second function template
         * is not ignored.
         */
        std::allocator<int> x;
        // std::cout << len1::len(x) << std::endl;     // ERROR: len1::len() function found, but can't size()
    }
    std::cout << "----------------------------\n";
    {
        int a[10];
        std::cout << len2::len(a) << std::endl;     // OK: len2::len() for array is best match
        std::cout << len2::len("tmp") << std::endl; // OK: len2::len() for array is best match

        std::vector<int> v;
        std::cout << len2::len(v) << std::endl;     // OK: len2::len() for a type with size_type is best

        /**
         * In len2 namespace, we also provide a general len2::len() function that always matches but has the
         * worst match (match with ellipsis(...) in overload resolution). So, for raw arrays and vectors, we
         * have two matches where the specific match is the better match. For pointers, only the fallback matches
         * so that the compiler no longer complains about a missing len2::len() for this call.
         */
        int* p;
        std::cout << len2::len(p) << std::endl;     // OK: only fallback len2::len() matches

        /**
         * But for the allocator, the second and third function templates match, with the second function template
         * as the better match. So, still, this results in an error that no size() member function can be called.
         */
        std::allocator<int> x;
        // std::cout << len2::len(x) << std::endl;      // ERROR: 2nd len2::len() function matches best,
                                                        //        but can't call size() for x
    }
    std::cout << "----------------------------\n";
    {
        std::allocator<int> x;
        // std::cout << len3::len(x) << std::endl;      // ERROR: no matching len3::len() function found
                                                        // instead of not calling size() for x
    }
}