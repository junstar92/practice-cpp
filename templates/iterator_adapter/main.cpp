/** Facade technique using CRTP (The Curiously Recurring Template Pattern) and Barton-Nackman Trick
 * Implementing IteratorFacade, IteratorFacadeAccess and ProjectionIterator.
 * An iterator adapter called ProjectionIterator that allows us "project" the values of an
 * underlying (base) iterator to some pointer-to-data member, for example, Person::firstName.
 */
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>

class IteratorFacadeAccess
{
    // only IteratorFacade can use these definition
    template<typename Derived, typename Value, typename Category,
             typename Reference, typename Distance>
    friend class IteratorFacade;

    // required of all iterators:
    template<typename Reference, typename Iterator>
    static Reference dereference(Iterator const& i) {
        return i.dereference();
    }
    template<typename Iterator>
    static bool equals(Iterator& il, Iterator& ir) {
        return il.equals(ir);
    }
    template<typename Iterator>
    static void increment(Iterator& i) {
        return i.increment();
    }
    // required of bidirectional iterators:
    template<typename Iterator>
    static void decrement(Iterator& i) {
        return i.decrement();
    }
    // required of random-access iterators:
    template<typename Iterator, typename Distance>
    static void advance(Iterator& i, Distance n) {
        return i.advance(n);
    }
};

template<typename Derived, typename Value, typename Category,
    typename Reference = Value&, typename Distance = std::ptrdiff_t>
class IteratorFacade
{
public:
    using value_type = typename std::remove_const<Value>::type;
    using reference = Reference;
    using pointer = Value*;
    using difference_type = Distance;
    using iterator_category = Category;

    Derived& asDerived() {
        return *static_cast<Derived*>(this);
    }
    Derived const& asDerived() const {
        return *static_cast<Derived const*>(this);
    }

    // input iterator interface
    reference operator*() const {
        return IteratorFacadeAccess::dereference<Reference>(asDerived());
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
        return !(lhs == rhs);
    }
};

struct Person {
    std::string firstName;
    std::string lastName;

    friend std::ostream& operator<<(std::ostream& os, Person const& p) {
        return os << p.lastName << ", " << p.firstName;
    }
};

template<typename Iterator, typename T>
class ProjectionIterator
    : public IteratorFacade<
        ProjectionIterator<Iterator, T>,
        T,
        typename std::iterator_traits<Iterator>::iterator_category,
        T&,
        typename std::iterator_traits<Iterator>::difference_type>
{
    using Base = typename std::iterator_traits<Iterator>::value_type;
    using Distance = typename std::iterator_traits<Iterator>::difference_type;

    Iterator iter;
    T Base::* member;

    friend class IteratorFacadeAccess;

    // implement core iterator operations for IteratorFacade
    T& dereference() const {
        return (*iter).*member;
    }
    void increment() {
        ++iter;
    }
    bool equals(ProjectionIterator const& other) const {
        return iter == other.iter;
    }
    void decrement() {
        --iter;
    }

public:
    ProjectionIterator(Iterator iter, T Base::* member)
    : iter(iter), member(member) {}
};

template<typename Iterator, typename Base, typename T>
auto project(Iterator iter, T Base::* member) {
    return ProjectionIterator<Iterator, T>(iter, member);
}

int main()
{
    std::vector<Person> authors = { {"Daivd", "Vandevoorde"},
                                    {"Nicolai", "Josuttis"},
                                    {"Douglas", "Gregor"}};
    // prints only firstName
    std::copy(project(authors.begin(), &Person::firstName),
        project(authors.end(), &Person::firstName),
        std::ostream_iterator<std::string>(std::cout, "\n"));
}