#pragma once
#include <type_traits>

template<typename Derived, typename Value, typename Category,
         typename Reference = Value&, typename Distance = std::ptrdiff_t>
class IteratorFacade;

// 'friend' this class allow IteratorFacade access to core iterator operations
class IteratorFacadeAccess
{
    // only IteratorFacade can use these definition
    template<typename Derived, typename Value, typename Category,
             typename Reference, typename Distance>
    friend class IteratorFacade;

    // input iterator interface
    template<typename Reference, typename Iterator>
    static Reference dereference(Iterator const& i) {
        return i.dereference();
    }
    template<typename Iterator>
    static void increment(Iterator& i) {
        i.increment();
    }
    template<typename Iterator>
    static bool equals(Iterator const& li, Iterator const& ri) {
        return li.equals(ri);
    }
};

template<typename Derived, typename Value, typename Category,
         typename Reference, typename Distance>
class IteratorFacade
{
public:
    using value_type = std::remove_const_t<Value>;
    using reference = Reference;
    using pointer = Value*;
    using difference_type = Distance;
    using iterator_category = Category;

    // input iterator interface
    reference operator*() const {
        return IteratorFacadeAccess::dereference<reference>(asDerived());
    }
    Derived& operator++() {
        IteratorFacadeAccess::increment(asDerived());
        return asDerived();
    }
    Derived operator++(int) {
        Derived result(asDerived());
        IteratorFacadeAccess::increment(asDerived());
        return result;
    }

    friend bool operator==(IteratorFacade const& lhs, IteratorFacade const& rhs) {
        return IteratorFacadeAccess::equals(lhs.asDerived(), rhs.asDerived());
    }
    friend bool operator!=(IteratorFacade const& lhs, IteratorFacade const& rhs) {
        return !(IteratorFacadeAccess::equals(lhs.asDerived(), rhs.asDerived()));
    }

private:
    Derived& asDerived() { return *static_cast<Derived*>(this); }
    Derived const& asDerived() const {
        return *static_cast<Derived const*>(this);
    }
};