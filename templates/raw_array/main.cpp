/** Templates for Raw Arrays and String Literals
 * 
 * When passing raw arrays or string literals to templates, some care has to be taken.
 * First, if the template parameters are declared as references, the arguments don't decay.
 * That is, a passed argument of "hello" has type `char const[6]`. This can become a problem
 * if raw arrays or string arguments of different length are passed because the types differ.
 * Only when passing the argument by value, the types decay, so that string literals are converted
 * to type `char const*`.
 * 
 */
#include <iostream>

template<typename T, int N, int M>
bool less(T(&a)[N], T(&b)[M]) {
    for (int i = 0; i < N && i < M; i++) {
        if (a[i] < b[i]) return true;
        if (b[i] < a[i]) return false;
    }

    return N < M;
}

template<int N, int M>
bool less(char const(&a)[N], char const(&b)[M]) {
    for (int i = 0; i < N && i < M; i++) {
        if (a[i] < b[i]) return true;
        if (b[i] < a[i]) return false;
    }

    return N < M;
}

template<typename T>
struct MyClass;

template<typename T, std::size_t SIZE>
struct MyClass<T[SIZE]> // partial specialization for arrays of known bounds
{
    static void print() {
        std::cout << "print() for T[" << SIZE << "]\n";
    }
};

template<typename T, std::size_t SIZE>
struct MyClass<T(&)[SIZE]> // partial specialization for references to arrays of known bounds
{
    static void print() {
        std::cout << "print() for T(&)[" << SIZE << "]\n";
    }
};

template<typename T>
struct MyClass<T[]> // partial specialization for arrays of unknown bounds
{
    static void print() {
        std::cout << "print() for T[]\n";
    }
};

template<typename T>
struct MyClass<T(&)[]> // partial specialization for references to arrays of unknown bounds
{
    static void print() {
        std::cout << "print() for T(&)[]\n";
    }
};

template<typename T>
struct MyClass<T*> // partial specialization for pointers
{
    static void print() {
        std::cout << "print() for T*\n";
    }
};

int main()
{
    {
    // example 1: less op on array
    int x[] = {1, 2, 3};
    int y[] = {1, 2, 3, 4, 5};
    // less<> is instantiated with T being int, N being 3, and M being 5
    std::cout << std::boolalpha << less(x, y) << "\n"; // print true    
    }

    {
    // example 2: less op on string
    char str1[] = "abcdef";
    char str2[] = "abcd";
    std::cout << less(str1, str2) << "\n"; // print false
    }

    {
    // example 3
    auto foo = [](int a1[7], int a2[],  // pointers by laungage rules
                  int (&a3)[42],        // reference to array of known bound
                  int (&x0)[],          // reference to array of unknown bound
                  auto x1,              // passing by value decays
                  auto& x2, auto&& x3)  // passing by reference
    {
        MyClass<decltype(a1)>::print(); // uses MyClass<T*>
        MyClass<decltype(a2)>::print(); // uses MyClass<T*>
        MyClass<decltype(a3)>::print(); // uses MyClass<T(&)[SIZE]>
        MyClass<decltype(x0)>::print(); // uses MyClass<T(&)[]>
        MyClass<decltype(x1)>::print(); // uses MyClass<T*>
        MyClass<decltype(x2)>::print(); // uses MyClass<T(&)[]>
        MyClass<decltype(x3)>::print(); // uses MyClass<T(&)[]>
    };
    int a[42];
    MyClass<decltype(a)>::print();  // uses MyClass<T[SIZE]>

    extern int x[]; // forward declare array
    MyClass<decltype(x)>::print();  // uses MyClass<T[]>

    foo(a, a, a, x, x, x, x);
    }
}

int x[] = {0, 8, 15}; // define forward-declared array