#pragma once
#include "try_equals.h"

// forward declaration for FunctorBridge
template<typename R, typename... Args>
class FunctorBridge;

/**
 * The interface to FunctionPtr is fairly straighforward, providing construction, copy, move, destruction,
 * initialization, and assignment from arbitrary function objects and invocation of the underlying function
 * object. The most interesting part of the interface is how it is described entirely within a class template
 * partial specialization, which serves to break the template argument (a function type) into its component
 * pieces (result and argument types):
 */
// primary template:
template<typename Signature>
class FunctionPtr;

// partial specialization:
template<typename R, typename... Args>
class FunctionPtr<R(Args...)>
{
private:
    /**
     * The implementation contains a single nonstatic member variable, bridge, which will be responsible for
     * both storage and manipulation of the stored function object. Ownership of this pointer is tied to the
     * FunctionPtr object, so most of the implementation provided merely manages this pointer.
     */
    FunctorBridge<R, Args...>* bridge;

public:
    // constructors:
    FunctionPtr() : bridge(nullptr) {}
    FunctionPtr(FunctionPtr const& other); // a definition is below
    FunctionPtr(FunctionPtr& other) : FunctionPtr(static_cast<FunctionPtr const&>(other)) {}
    FunctionPtr(FunctionPtr&& other) : bridge(other.bridge) {
        other.bridge = nullptr;
    }

    // constructor from arbitrary function objects:
    template<typename F>
    FunctionPtr(F&& f); // a definition is below

    // assignment operators:
    FunctionPtr& operator=(FunctionPtr const& other) {
        FunctionPtr tmp(other);
        swap(*this, tmp);
        return *this;
    }
    FunctionPtr& operator=(FunctionPtr&& other) {
        delete bridge;
        bridge = other.bridge;
        other.bridge = nullptr;
        return *this;
    }

    // construction and assignment from arbitrary function objects:
    template<typename F>
    FunctionPtr& operator=(F&& f) {
        FunctionPtr tmp(std::forward<F>(f));
        swap(*this, tmp);
        return *this;
    }

    // destructor
    ~FunctionPtr() {
        delete bridge;
    }

    friend void swap(FunctionPtr& fp1, FunctionPtr& fp2) {
        std::swap(fp1.bridge, fp2.bridge);
    }

    explicit operator bool() const {
        return bridge == nullptr;
    }
    // invocation:
    R operator()(Args... args) const; // a definition is below

    friend bool operator==(FunctionPtr const& f1, FunctionPtr const& f2) {
        if (!f1 || !f2) {
            return !f1 && !f2;
        }
        return f1.bridge->equals(f2.bridge);
    }
    friend bool operator!=(FunctionPtr const& f1, FunctionPtr const& f2) {
        return !(f1 == f2);
    }
};

/** Bridge Interface
 * The FunctorBridge class template is responsible for the ownership and manipulation of the underlying
 * function object. It is implemented as an abstract base class, forming the foundation for the dynamic
 * polymorphism of FunctionPtr.
 * FunctorBridge provides the essential operations needed to manipulate a stored function object through
 * virtual functions: a destructor, a clone() operation to perform copies, and an invoke() operation to be
 * const member functions.
 */
template<typename R, typename... Args>
class FunctorBridge
{
public:
    virtual ~FunctorBridge() {}
    virtual FunctorBridge* clone() const = 0;
    virtual R invoke(Args... args) const = 0;
    virtual bool equals(FunctorBridge const* fb) const = 0;
};

/**
 * Using virtual function of FunctorBridge above, we can implement FunctionPtr's copy constructor and
 * function call operator:
 */
template<typename R, typename... Args>
FunctionPtr<R(Args...)>::FunctionPtr(FunctionPtr const& other) : bridge(nullptr)
{
    if (other.bridge) {
        bridge = other.bridge->clone();
    }
}
template<typename R, typename... Args>
R FunctionPtr<R(Args...)>::operator()(Args... args) const
{
    return bridge->invoke(std::forward<Args>(args)...);
}

/** Type Erasure
 * Each instance of FunctorBridge is an abstract class, so its derived classes are responsible for providing
 * actual implementations of its virtual functions. To support the complete range of potential function objects
 * - an unbounded set - we would need an unbounded member of derived classes. Fortunately, we can accomplish
 * this by parameterizing the derived class on the type of the function object it stores.
 */
template<typename Functor, typename R, typename... Args>
class SpecificFunctorBridge : public FunctorBridge<R, Args...>
{
    Functor functor;
public:
    template<typename FunctorFwd>
    SpecificFunctorBridge(FunctorFwd&& functor) : functor(std::forward<FunctorFwd>(functor)) {}
    virtual SpecificFunctorBridge* clone() const override {
        return new SpecificFunctorBridge(functor);
    }
    virtual R invoke(Args... args) const override {
        return functor(std::forward<Args>(args)...);
    }
    virtual bool equals(FunctorBridge<R, Args...> const* fb) const override {
        if (auto const* specFb = dynamic_cast<SpecificFunctorBridge const*>(fb)) {
            return TryEquals<Functor>::equals(functor, specFb->functor);
        }
        return false;
    }
};

/**
 * Each instance of SpecificFunctorBridge stores a copy of the function object (whose type is Functor), which
 * can be invoked, copied (by cloning the SpecificFunctorBridge), or destroyed (implicitly in the destructor).
 * SpecificFunctorBridge instances are created whenever a FuncionPtr is initialized to a new function object,
 * completing the FunctionPtr's constructor for arbitrary function objects.
 * 
 * Note that while the FunctionPtr constructor itself is templated on the function object type F, that type is
 * known only to the particular specialization of SpecificFunctorBridge (described by the Bridge type alias).
 * Once the newly allocated Bridge instance is assigned to the data member bridge, the extra information about
 * the specific type F is lost due to the derived-to-based conversion from Bridge* to FunctorBridge<R, Args...>*.
 * This loss of type information explains why the term type erasure is often used to describes the technique of
 * bridging between static and dynamic polymorphism.
 */
template<typename R, typename... Args>
template<typename F>
FunctionPtr<R(Args...)>::FunctionPtr(F&& f) : bridge(nullptr)
{
    // It uses std::decay to produce the Functor type, which makes the inferred type F suitable for storage, for
    // example, by tuning references to function types info function pointer types and removing top-level const,
    // volatile, and reference types.
    using Functor = std::decay_t<F>;
    using Bridge = SpecificFunctorBridge<Functor, R, Args...>;
    bridge = new Bridge(std::forward<F>(f));
}