#include <iostream>

/**
 * The main restriction on the CRTP is that the size of the base class, B, cannot depend on its template
 * parameter, D. More generally, the template for class B has to instantiate with type D being an incomplete
 * type. For example, this will not compile:
```
template<typename D>
class B
{
    using T = typename D::T;
    T* p_;
};

class D : public B<D>
{
    using T = int;
};
```
 * The problem lies not with class B itself, but with our intended use of it:
class D : public B<D> ...
 * 
 * At the point where B<D> has to be known, type D has not been declared yet. It cannot be class D is forward-
 * declared, the same as if we had this code:
```
class A;
B<A> b;  // does not compile
```
 * The declaration of class D requires us to know exactly what the base class B<D> is. So, if class D has
 * not been declared yet, how does the compiler know that the indentified D even refers to a type ?
 * After all, you cannot instantiate a template on a completely unknown type. The answer lies somewhere in between.
 * 
 * Some templates can be instantiated on forward-declared types, while others cannot. The exact rules can be painstackingly
 * gathered from the standard, but the gist is this - anything that might affect the size of the class has to be
 * fully declared. A reference to a type declared inside an incomplete type, such as `using T = typename D::T`,
 * would be a forward declaration of a nested class, and those are not allowed either.
 * 
 * On the other hand, the body of a member function of a class template is not instantiated until it's called.
 * In fact, for a given template parameter, a member function does not even have to compile, as long as it's not
 * called. Therefore, references to the derived class, its nested types, and its member functions, inside the member
 * functions of the base class are perfectly fine. Also, since the derived class type is considered forward-declared
 * inside the base class, we can declare pointers and refereces to it. Here is a very common refactoring of the CRTP
 * base class that consolidates the uses of the static cast in one place:
 */
template<typename B>
class  A {
public:
    A() : i_{0} {}
    virtual ~A() {}
    void f(int i) { derived()->f(i); }
    B* derived() { return static_cast<B*>(this); }

protected:
    int i_{};
};

class B : public A<B> {
    void f(int i) { i_ += i; }
};

template<typename D, typename value_type>
class C {
public:
    // typename D::result_type f() { return i; }        // incomplete type - does not compile
    auto f() {
        using res_t = typename D::result_type;          // OK inside function body
        return res_t{i};                                // Decuding auto == res_t
    }
    // typename B::value_type i;                        // incomplete type - does not compile
    value_type i{};
};

class D : public C<D, int> {
public:
    using result_type = int;
    using value_type = int;
};

int main()
{
    D d;
    d.i = 1;
    std::cout << d.f() << std::endl;
}