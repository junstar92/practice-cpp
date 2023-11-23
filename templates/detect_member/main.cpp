/** Detecting Members
 * By using SFINAE-based traits, we can create a trait that can determine whether a given type T has
 * a member of given name X (a type or a nontype member). 
 */
#include <iostream>
#include <type_traits>
#include <vector>

namespace detect_member {
/** 1. Deteting Member Types
 * Let's first define a trait that can determine whether a given type T has a member type size_type.
 * Here, we use the approach to SFINAE out partial specialization. As usual for predicate traits,
 * we define the general case to be derived from std::false_type, because by default a type doesn't
 * have the member size_type. In this case, we only need one construct: `typename T::size_type`.
 * This construct is valid if and only if type T has a member type size_type, which is exactly what
 * we are trying to determine. If, for a specific T, the construct is invalid (i.e., type T has no
 * member type size_type), SFINAE causes the partial specialization to be discarded, and we fall back
 * to the primary template. Otherwise, the partial specialization is valid and preferred.
 */
namespace v1 {
// primary template
template<typename, typename = std::void_t<>>
struct HasSizeTypeT : std::false_type {};
// partial specialization (may be SFINAE'd away)
template<typename T>
struct HasSizeTypeT<T, std::void_t<typename T::size_type>> : std::true_type {};
}

namespace v2 {
// primary template
template<typename, typename = std::void_t<>>
struct HasSizeTypeT : std::false_type {};
// partial specialization (may be SFINAE'd away)
template<typename T>
struct HasSizeTypeT<T, std::void_t<typename std::remove_reference<T>::type::size_type>> : std::true_type {};
}

void test() {
    std::cout << std::boolalpha << v1::HasSizeTypeT<int>::value << std::endl; // false
    struct CX {
        using size_type = std::size_t;
    };
    std::cout << v1::HasSizeTypeT<CX>::value << std::endl; // true


    // With a traits template like v1::HasSizeTypeT, interesting issues can arise with reference types.
    // For example, while the following works fine.
    struct CXR {
        using size_type = char&; // Note: type size_type is a reference type
    };
    std::cout << v1::HasSizeTypeT<CXR>::value << std::endl; // OK: prints true
    // but, the following fails.
    std::cout << v1::HasSizeTypeT<CX&>::value << std::endl;  // OOPS: prints false
    std::cout << v1::HasSizeTypeT<CXR&>::value << std::endl; // OOPS: prints false

    // By using std::remove_reference trait in the partial specialization of HasSizeTypeT, it can solve
    // this problem
    std::cout << v2::HasSizeTypeT<CX&>::value << std::endl;  // OK: prints true
    std::cout << v2::HasSizeTypeT<CXR&>::value << std::endl; // OK: prints true
}

}

/** 2. Detecting Arbitrary Member Types
 * Defining a trait such as HasSizeTypeT raises the question of how to parameterize the trait to be able
 * to check for any member type name.
 * Unfortunately, this can currently be achieved only via macros, because there is no lanugage mechanism
 * to describe a "potential" name. The closet we can get for the moment without using macros is to use generic
 * lambdas.
 */
#define DEFINE_HAS_TYPE(MemType)                                \
template<typename, typename = std::void_t<>>                    \
struct HasTypeT_##MemType                                       \
    : std::false_type {};                                       \
template<typename T>                                            \
struct HasTypeT_##MemType<T, std::void_t<typename T::MemType>>  \
    : std::true_type {}

namespace detect_arbitrary_member {

DEFINE_HAS_TYPE(value_type);
DEFINE_HAS_TYPE(char_type);

void test() {
    std::cout << "int::value_type: " << HasTypeT_value_type<int>::value << std::endl;
    std::cout << "std::vector<int>::value_type: " << HasTypeT_value_type<std::vector<int>>::value << std::endl;
    std::cout << "std::iostream::value_type: " << HasTypeT_value_type<std::iostream>::value << std::endl;
    std::cout << "std::iostream::char_type: " << HasTypeT_char_type<std::iostream>::value << std::endl;
}

}

/** 3. Detecting Nontype Members
 * We can modify the trait to also check for data members and (single) member functions.
 * Here, we use SFINAE to disable the partial specialization when &T::Member is not valid. For that construct
 * to be valid, the following must be true.
 * - Member must unambiguously identify a member of T (e.g., it cannot be an overloaded member function name, or
 *   the name of multiple inherited members of the same name).
 * - The member must be accessible.
 * - The member must be a nontype, nonenumerator member (otherwise the prefix & would be invalid).
 * - If T::Member is a static data member, its type must not provide an operator& that makes &T::Member invalid
 *   (e.g., by making that operator inaccessible).
 * It would be not be difficult to modify the partial specialization to exclude cases when &T::Member is not a
 * pointer-to-member type (which amounts to excluding static data members). Similarly, a pointer-to-member function
 * could be excluded or required to limit the trait to data members or member functions.
 */
#define DEFINE_HAS_MEMBER(Member)                               \
template<typename, typename = std::void_t<>>                    \
struct HasMemberT_##Member                                      \
    : std::false_type {};                                       \
template<typename T>                                            \
struct HasMemberT_##Member<T, std::void_t<decltype(&T::Member)>>\
    : std::true_type {}

namespace detect_nontype_member {

DEFINE_HAS_MEMBER(size);
DEFINE_HAS_MEMBER(first);
DEFINE_HAS_MEMBER(begin);

void test() {
    std::cout << "int::size: " << HasMemberT_size<int>::value << std::endl;
    std::cout << "std::vector<int>::size: " << HasMemberT_size<std::vector<int>>::value << std::endl;
    std::cout << "std::pair<int, int>::first: " << HasMemberT_first<std::pair<int,int>>::value << std::endl;
    std::cout << "std::vector<int>::begin: " << HasMemberT_begin<std::vector<int>>::value << std::endl; // OOPS: prints false but true is expected.
                                                                                                        // this problem is covered below 3-1.
}

}

/** 3-1. Detecting Member Functions
 * Note that the HasMember trait only checks whether a single member with the corresponding name exists. The
 * trait also will fail if two members exists, which might happen we check for overloaded member functions.
 * Refer to line 125 in detect_nontype_member::test() function.
 * However, the SFINAE principle protects against attempts to create both invalid types and expressions in a
 * function template declaration, allowing the overloading technique above to extend to testing whether arbitrary
 * expressions are well-formed.
 * That is, we can simply check whether we can call a function of interest in specific way and that can succeed
 * even if the function is overloaded. The trick is to formulate the expression that checks whether we can call
 * begin() inside a decltype expression for the default value of an additional function template parameter.
 * 
 * Here, we use `decltype(stD::declval<T>().begin())` to test whether, given a value/object of type T, calling a
 * member begin() is valid (using std::declval to avoid any constructor being reauired).
 */
namespace detect_member_func {

// primary template
template<typename, typename = std::void_t<>>
struct HasBeginT : std::false_type {};
// partial specialization (may be SFINAE'd away)
template<typename T>
struct HasBeginT<T, std::void_t<decltype(std::declval<T>().begin())>> : std::true_type {};

void test() {
    std::cout << "std::vector<int>::begin: " << HasBeginT<std::vector<int>>::value << std::endl; // OK: prints true
}

}

/** 3-2. Detecting Other Expressions
 * We can use the technique above for other kinds of expressions and even combine multiple expressions. For
 * example, we can test whether, given types T1 and T2, there is a suitable < operator defined for values of
 * these types.
 * 
 * We can use this trait to require that a template parameter T supports operator <.
 * ```
 * template<typename T>
 * class C {
 *   static_assert(HasLessT<T>::value, "Class C requires comparable elements");
 *   ...
 * };
 * ```
 * 
 * Note that, due to the nature of std::void_t, we can combine multiple constraints in a trait. Refer to
 * HasVariousT trait.
 */
#include <string>
#include <complex>
namespace detect_others {

// primary template
template<typename, typename, typename = std::void_t<>>
struct HasLessT : std::false_type {};
// partial specialization (may be SFINAE'd away)
template<typename T1, typename T2>
struct HasLessT<T1, T2, std::void_t<decltype(std::declval<T1>() < std::declval<T2>())>> : std::true_type {};

// multiple constraints in a trait
template<typename, typename = std::void_t<>>
struct HasVariousT : std::false_type {};
template<typename T>
struct HasVariousT<T, std::void_t<decltype(std::declval<T>().begin()),
                                  typename T::difference_type,
                                  typename T::iterator>> : std::true_type {};

void test() {
    std::cout << HasLessT<int, char>::value << std::endl;                                   // prints true
    std::cout << HasLessT<std::string, std::string>::value << std::endl;                    // prints true
    std::cout << HasLessT<std::string, int>::value << std::endl;                            // prints false
    std::cout << HasLessT<std::string, char*>::value << std::endl;                          // prints true
    std::cout << HasLessT<std::complex<double>, std::complex<double>>::value << std::endl;  // prints false
}

}

/** 4. Using Generic Lambdas to Detect Members
 * The following example illustrates how to define traits checking whether a data or type member such as
 * first or size_type exists or whether operator< is defined for two objects of different types.
 */
namespace use_lambda {

// helper to check validity of f(args...) for F f and Args... args
template<typename F, typename... Args,
         typename = decltype(std::declval<F>()(std::declval<Args&&>()...))>
std::true_type is_valid_impl(void*);
template<typename F, typename... Args>
std::false_type is_valid_impl(...);
// define a lambda that takes a lambda f and returns whether calling f with args is valid
inline constexpr
auto is_valid = [](auto f) {
    return [](auto&&... args) {
        return decltype(is_valid_impl<decltype(f),
                                      decltype(args)&&...
                                     >(nullptr)
        ) {};
    };
};
// helper template to represent a type as a value
template<typename T>
struct TypeT { using Type = T; };
// helper to wrap a type as a value
template<typename T>
constexpr auto type = TypeT<T>{};
// helper to unwrap a wrapped type in unvaluated contexts
template<typename T>
T valueT(TypeT<T>); // no definition needed

constexpr auto has_first
    = is_valid([](auto x) -> decltype((void)valueT(x).first) {});
constexpr auto has_size_type
    = is_valid([](auto x) -> typename decltype(valueT(x))::size_type {});
constexpr auto has_less
    = is_valid([](auto x, auto y) -> decltype(valueT(x) < valueT(y)) {});

void test() {
    std::cout << "has_first: " << has_first(type<std::pair<int, int>>) << std::endl;    // true
    struct CX {
        using size_type = std::size_t;
    };
    std::cout << "has_size_type: " << has_size_type(type<CX>) << std::endl;             // true
    std::cout << has_less(42, type<char>) << "\n";                                      // true but prints false
    std::cout << has_less(type<std::string>, type<std::string>) << "\n";                // true
    std::cout << has_less(type<std::string>, type<int>) << "\n";                        // false
    std::cout << has_less(type<std::string>, "hello") << "\n";                          // true but prints false
}

/**
 * Note that has_size_type uses std::decay to remove the references from the passed x because you can't
 * access a type member from a reference. If you skip that, the traits will always field false because the
 * second overload of is_valid_impl<>() is used. (refer to line 251 and 254)
 * To be able to use the common generic syntax, taking types as template parameters, we can again define
 * additional helpers. Refer to below.
 */
constexpr auto has_first_v2
    = is_valid([](auto&& x) -> decltype((void)&x.first) {});
template<typename T>
using HasFirstT = decltype(has_first_v2(std::declval<T>()));
constexpr auto has_size_type_v2
    = is_valid([](auto&& x) -> typename std::decay_t<decltype(x)>::size_type {});
template<typename T>
using HasSizeTypeT = decltype(has_size_type_v2(std::declval<T>()));
constexpr auto has_less_v2
    = is_valid([](auto&& x, auto&& y) -> decltype(x < y) {});
template<typename T1, typename T2>
using HasLessT = decltype(has_less_v2(std::declval<T1>(), std::declval<T2>()));

void test2() {
    std::cout << "has_first: " << HasFirstT<std::pair<int, int>>::value << std::endl;   // true
    struct CX {
        using size_type = std::size_t;
    };
    std::cout << "has_size_type: " << HasSizeTypeT<CX>::value << std::endl;             // true
    std::cout << HasLessT<int, char>::value << "\n";                                    // true
    std::cout << HasLessT<std::string, std::string>::value << "\n";                     // true
    std::cout << HasLessT<std::string, int>::value << "\n";                             // false
    std::cout << HasLessT<std::string, char*>::value << "\n";                           // true
}

}

int main()
{
    std::cout << "----------------------------\n";
    detect_member::test();
    std::cout << "----------------------------\n";
    detect_arbitrary_member::test();
    std::cout << "----------------------------\n";
    detect_nontype_member::test();
    std::cout << "----------------------------\n";
    detect_member_func::test();
    std::cout << "----------------------------\n";
    detect_others::test();
    std::cout << "----------------------------\n";
    use_lambda::test();
    std::cout << "----------------------------\n";
    use_lambda::test2();
}