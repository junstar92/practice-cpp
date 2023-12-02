#include <iostream>
#include <cstddef>

/**
 * ObjectCounter is a simple application of CRTP (The Curiously Recurring Template Pattern)
 * consists of keeping track of how many objects of a certain class type were created.
 * This is easily achieved by incrementing an integral static data member in every constructor
 * and decrementing it in the destructor.
 */
template<typename CountedType>
class ObjectCounter {
private:
    inline static std::size_t count = 0; // number of existing objects

protected:
    // default constructor
    ObjectCounter() {
        ++count;
    }
    // copy constructor
    ObjectCounter(ObjectCounter<CountedType> const&) {
        ++count;
    }
    // move constructor
    ObjectCounter(ObjectCounter<CountedType>&&) {
        ++count;
    }
    // destructor
    ~ObjectCounter() {
        --count;
    }

public:
    // return number of existing objects
    static std::size_t live() {
        return count;
    }
};

class MyClass1 : public ObjectCounter<MyClass1>
{
public:
    MyClass1() {
        std::cout << "[ctor] the number of MyClass1's objects: " << live() << std::endl;
    }
    ~MyClass1() {
        std::cout << "[dtor] the number of MyClass1's objects: " << live() << std::endl;
    }
};

template<typename T>
class MyClass2 : public ObjectCounter<MyClass2<T>>
{
public:
    MyClass2() {
    }
};

int main()
{
    MyClass1 a, b;

    MyClass2<char> d, e, f;
    MyClass2<int> g, h, i, j;

    std::cout << "the number of MyClass2<char>'s objects: " << MyClass2<char>::live() << std::endl;
    std::cout << "the number of MyClass2<int>'s objects: " << MyClass2<int>::live() << std::endl;
    
    return 0;
}