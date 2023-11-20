/**
 * The keyword 'typename' was introduced during the standardization of C++ to clarify that
 * an identifier inside a template is a type.
 * 
 * Example:
 * ```
 * template<typename T>
 * class MyClass {
 * public:
 *     ...
 *     void foo() {
 *         typename T::SubType* ptr;
 *     }
 * };
 * ```
 * Here, the second typename is used to clarify that SubType is a type defined within class T.
 * Thus, ptr is a pointer to the type T::SubType. Without typename, SubType would be assumed to
 * be a nontype member (e.g., a static data member or an enumerator constant). As a result, 
 * the expression: `T::SubType* ptr` would be a multiplication of the static SubType member of 
 * class T with ptr, which is not an error, because for some instantiations of MyClass<> this could
 * be valid code.
 */
#include <iostream>
#include <vector>

// print elements of an STL container
template<typename T>
void print_coll(T const& coll) {
    typename T::const_iterator pos;
    typename T::const_iterator end(coll.end());
    for (pos = coll.begin(); pos != end; pos++) {
        std::cout << *pos << ' ';
    }
    std::cout << "\n";
}

int main()
{
    std::vector<int> coll{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    print_coll(coll);
}