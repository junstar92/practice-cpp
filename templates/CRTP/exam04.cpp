/** CRTP as Delegation Pattern
 * 
 * In exam01~04, we have used CRTP as a compile-time equivalent of dynamic polymorphism, including
 * virtual-like calls through the base pointer (compile-time, of course, with a template function).
 * This is not the only way CRTP can be used. In face, more often than not, the function is called
 * directly on the drived class. This is a very fundamental difference - typically, public inheritance
 * expresses the is-a relationship - the derived object is a kind of a base object. The interface and
 * generic code are in the base class, while the derived class overrides the specific implementation.
 * This relation continues to hold when a CRTP object is accessed through the base class pointer or
 * reference. Such use of CRTP is sometimes also called a static interface.
 */
#include <iostream>

/** Expanding the interface 
 * Let's consider several examples where CRTP is used to delegate behavior from the base class to the derived one.
 */

namespace expand_interface1 {

template<typename D>
struct plus_base {
    D operator+(D const& rhs) const {
        D tmp = rhs;
        tmp += static_cast<D const&>(*this);
        return tmp;
    }
};

class D : public plus_base<D> {
    int i_;

public:
    explicit D(int i) : i_{i} {}
    D& operator+=(D const& rhs) {
        i_ += rhs.i_;
        return *this;
    }
    friend std::ostream& operator<<(std::ostream& os, D const& d) {
        std::cout << d.i_;
        return os;
    }
};

}

namespace expand_interface2 {

template<typename D>
struct plus_base {
    friend D operator+(D const& lhs, D const& rhs) {
        D tmp = lhs;
        tmp += rhs;
        return tmp;
    }
};

class D : public plus_base<D> {
    int i_;

public:
    explicit D(int i) : i_{i} {}
    D& operator+=(D const& rhs) {
        i_ += rhs.i_;
        return *this;
    }
    friend std::ostream& operator<<(std::ostream& os, D const& d) {
        std::cout << d.i_;
        return os;
    }
};

}

namespace expand_interface3 {
// for C++20
class D {
    int i_;
    public:
    explicit D(int i) : i_(i) {}
    auto operator<=>(D const& rhs) const {
        return (i_ < rhs.i_) ? -1 : ((i_ > rhs.i_) ? 1 : 0);
    }
    bool operator==(D const& rhs) const {
        return i_ == rhs.i_;
    }
    friend std::ostream& operator<<(std::ostream& out, D const& d) { out << d.i_; return out; }
};

}

int main()
{
    {
        expand_interface1::D d1(1);
        expand_interface1::D d2(7);
        expand_interface1::D d3(42);

        std::cout << "d1: " << d1 << ", d2: " << d2 << ", d3: " << d3 << std::endl;
        std::cout << "d1 + d2: " << (d1 + d2) << std::endl;
        std::cout << "d1 + d3: " << (d1 + d3) << std::endl;
    }
    {
        expand_interface2::D d1(1);
        expand_interface2::D d2(7);
        expand_interface2::D d3(42);

        std::cout << "d1: " << d1 << ", d2: " << d2 << ", d3: " << d3 << std::endl;
        std::cout << "d1 + d2: " << (d1 + d2) << std::endl;
        std::cout << "d1 + d3: " << (d1 + d3) << std::endl;
    }
    {
        expand_interface3::D d1(1);
        expand_interface3::D d2(7);
        expand_interface3::D d3(42);

        std::cout << "d1: " << d1 << ", d2: " << d2 << ", d3: " << d3 << std::endl;
        std::cout << "d1 == d2: " << (d1 == d2) << " " << std::endl;
        std::cout << "d1 <= d2: " << (d1 <= d2) << " " << std::endl;
        std::cout << "d1 <  d2: " << (d1 <  d2) << " " << std::endl;
        std::cout << "d1 == d3: " << (d1 == d3) << " " << std::endl;
        std::cout << "d1 <= d3: " << (d1 <= d3) << " " << std::endl;
        std::cout << "d1 <  d3: " << (d1 <  d3) << " " << std::endl;
    }
}