#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "iterator_facades.h"

template<typename T>
class ListNode
{
public:
    T value;
    ListNode<T>* next = nullptr;
    ~ListNode() { delete next; }
};

template<typename T>
class ListNodeIterator
: public IteratorFacade<ListNodeIterator<T>, T, std::forward_iterator_tag>
{
    friend class IteratorFacadeAccess;
    ListNode<T>* current = nullptr;

    T& dereference() const {
        return current->value;
    }
    void increment() {
        current = current->next;
    }
    bool equals(ListNodeIterator const& other) const {
        return current == other.current;
    }
public:
    ListNodeIterator(ListNode<T>* current = nullptr) : current(current) {}
};

struct Person {
    std::string firstName;
    std::string lastName;

    friend std::ostream& operator<<(std::ostream& os, Person const& p) {
        return os << p.lastName << ", " << p.firstName;
    }
};

template<typename Iterator, typename T>
class ProjectionIterator : public IteratorFacade<ProjectionIterator<Iterator, T>,
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

    T& dereference() const {
        return (*iter).*member;
    }
    void increment() {
        ++iter;
    }
    bool equals(ProjectionIterator const& other) const {
        return iter == other.iter;
    }

public:
    ProjectionIterator(Iterator iter, T Base::* member) : iter(iter), member(member) {}
};

template<typename Iterator, typename Base, typename T>
auto project(Iterator iter, T Base::* member) {
    return ProjectionIterator<Iterator, T>(iter, member);
}

int main()
{
    ListNode<int> head{1};
    head.next = new ListNode<int>{2};
    head.next->next = new ListNode<int>{3};

    auto it = ListNodeIterator(&head);
    std::cout << *it << std::endl;
    ++it;
    std::cout << *it << std::endl << std::endl;

    std::vector<Person> authors = {
        {"David", "Vandevoorde"},
        {"Nicolai", "Josuttis"},
        {"Douglas", "Gregor"}
    };
    std::copy(project(authors.begin(), &Person::firstName),
            project(authors.end(), &Person::firstName),
            std::ostream_iterator<std::string>(std::cout, "\n"));

    return 0;
}