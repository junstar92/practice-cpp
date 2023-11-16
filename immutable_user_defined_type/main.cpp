#include <iostream>
#include <ostream>

/**
 * The class MyInt has two constexpr constructors. A default constructor and a constructor
 * taking two arguments. Additionally, the class has two methods, but only the method get_sum
 * is a constant expression. I declared the method const because a constexpr method is in
 * contrast to C++11 with C++14 not automatically const. There are two ways to define the
 * variables my_val1 and my_val2 if I want to use them in constexpr object. First, I can 
 * initialize them in the initialization list of the constructor; second, I can initialize
 * them in the class body. The initialization in the initialization list of the constructor
 * has the higher priority.
 * 
 * Note: A constexpr object can only use constexpr methods.
 * 
 */
class MyInt
{
public:
    constexpr MyInt() = default;
    constexpr MyInt(int first, int second) : my_val1{first}, my_val2{second} {}
    MyInt(int i) {
        my_val1 = i - 2;
        my_val2 = i + 3;
    }

    constexpr int get_sum() const { return my_val1 + my_val2; }

    friend std::ostream& operator<<(std::ostream& os, MyInt const& my_int) {
        return os << "(" << my_int.my_val1 << "," << my_int.my_val2 << ")";
    }

private:
    int my_val1 = 1998;
    int my_val2 = 2003;
};

int main()
{
    std::cout << std::endl;

    constexpr MyInt my_int_const1;

    constexpr int sec = 2014;
    constexpr MyInt my_int_const2(2011, sec);
    std::cout << "my_int_const2.get_sum(): " << my_int_const2.get_sum() << std::endl;

    int arr[my_int_const2.get_sum()];
    static_assert(my_int_const2.get_sum() == 4025, "2011 + 2014 should be 4025");

    std::cout << std::endl;
}