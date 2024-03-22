/** Static Polymorphism
 * 
 * Since CRTP allows us to override base class functions with those of the derived class, it implements
 * polymorphic behavior. The key difference is that polymorphism happens at compile time, not at runtime.
 * 
 * What would be the equivalent of the pure virtual function in the CRTP scenario ? A pure virtual function
 * must be implemented in all derived classes. A class that declares a pure virtual function, or inherits
 * one and does not override it, is an abtract class; it can be further derived from, but it cannot be instantiated.
 * 
 * When we contemplate the equivalent of a pure virtual function for static polymorphism, we realize that
 * out CRTP implementation suffers from a major vulnerability. What happens if we forget to override the
 * compile-time virtual function, f(), in one of the derived classes ?
 */
#include <iostream>

namespace bad_crtp {

template<typename D>
class B {
public:
    B() : i_{0} {}
    virtual ~B() {}
    void f(int i) {
        std::cout << "B::f() calling D::f()...\n";
        static_cast<D*>(this)->f(i);
        std::cout << "B::f() done!\n";
    }
    int get() const { return i_; }

protected:
    int i_;
};

class D_bad : public B<D_bad> {
public:
    // No f() here! void f(int i) { i_ += i; }
};

class D_ok : public B<D_ok> {
public:
    void f(int i) {
        std::cout << "Running D::f()...\n";
        i_ += i;
        std::cout << "D::f() done\n";
    }
};

}

namespace good_crtp {

template<typename D>
class B {
public:
    B() : i_{0} {}
    virtual ~B() {}
    void f(int i) { // equivalent of a virtual with a default
        std::cout << "B::f() calling D::f()...\n";
        static_cast<D*>(this)->f_impl(i);
        std::cout << "B::f() done!\n";
    }
    void g(int i) { // equivalent of a pure virtual
        std::cout << "B::g() calling D::g()...\n";
        static_cast<D*>(this)->g_impl(i);
        std::cout << "B::g() done!\n";
    }
    int get() const { return i_; }

protected:
    void f_impl(int i) {
        std::cout << "Running B::f()...\n";
        i += i;
        std::cout << "B::f() done\n";
    }
    int i_;
};

class D1 : public B<D1> {
public:
    void f_impl(int i) {
        std::cout << "Running D1::f()..." << std::endl;
        i_ += i;
        std::cout << "D1::f() done" << std::endl;
    }
    void g_impl(int i) {
        std::cout << "Running D1::g()..." << std::endl;
        i_ += i;
        std::cout << "D1::g() done" << std::endl;
    }
};
class D2 : public B<D2> {
public:
    // No f_impl() here! void f_impl(int i) { i_ += i; }
    // No g_impl() here! void g_impl(int i) { i_ += i; }
};

}

template<typename D>
class B {
public:
    B() {}
    virtual ~B() { std::cout << "call ~B()\n"; }
};

class D : public B<D> {
public:
    D() {}
    virtual ~D() { std::cout << "call ~D()\n"; }
};

int main()
{
    {
        bad_crtp::B<bad_crtp::D_ok>* b = new bad_crtp::D_ok;
        b->f(42);
    }
    {
        bad_crtp::B<bad_crtp::D_bad>* b = new bad_crtp::D_bad;
        //b->f(42); // compile ok, but err: infinite loop ! 
    }
    {
        good_crtp::B<good_crtp::D1>* b = new good_crtp::D1;
        b->f(42);
        b->g(42);
    }
    {
        good_crtp::B<good_crtp::D2>* b = new good_crtp::D2;
        b->f(42);
        // b->g(42); // Should not compile
    }

    return 0;
}