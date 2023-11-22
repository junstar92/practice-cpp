/**
 * Class members can also be templates. This is possible for both nested classes and member functions.
 * The application and advantage of this ability can be demonstrated with the Stack<> class template.
 * Normally you can assign stasks to each other only when they have the same type, which implies that
 * the elements have the same type. However, you can't assign a stack with elements of any other type,
 * even if there is an implicit type conversion for the element type defined:
 * 
 * ```
 * Stack<int> int_stack1, int_stack2;
 * Stack<float> float_stack;
 * ...
 * int_stack1 = int_stack2;  // OK: stacks have same type
 * float_stack = int_stack1; // ERROR: stacks have different types
 * ```
 * 
 * The default assignment operator requires that both sides of the assignment operator have the same type,
 * which is not the case if stacks have different element types. However, by defining an assignment operator
 * as a template, you can enable the assignment of stacks with elements for which an appropriate type
 * conversion is defined.
 */
#include <iostream>
#include <deque>

template<typename T>
class Stack
{
private:
    std::deque<T> elems;

public:
    void push(T const& elem) {
        elems.push_front(elem);
    }
    void pop() {
        elems.pop_front();
    }
    T const& top() const {
        return elems.front();
    }
    bool empty() const {
        return elems.empty();
    }
    // assign stack of elements of type T2
    template<typename T2>
    Stack& operator=(Stack<T2> const& op2);
    // to get access to private members of Stack<T2> for any type T2
    template<typename> friend class Stack;
};

template<typename T>
template<typename T2>
Stack<T>& Stack<T>::operator=(Stack<T2> const& op2) {
    elems.clear();
    elems.insert(elems.begin(), op2.elems.begin(), op2.elems.end());
    return *this;
}

/**
 * Member function templates can also be partially or fully specialized. For example, for the following class:
 */
#include <string>

class BoolString
{
private:
    std::string value;

public:
    BoolString(std::string const& str) : value(str) {}

    template<typename T = std::string>
    T get() const {
        return value;
    }
};

// full specialization for BoolString::getValue<>() for bool
template<>
inline bool BoolString::get<bool>() const {
    return value == "true" || value == "1" || value == "on";
}

/**
 * Template member functions can be used wherever special member functions allow copying or moving objects.
 * Similar to assignment operatos as defined above, they can also be constructors. However, note that template
 * constructors or template assignment operators don't replace predefined constructors or assignment operators.
 * Member templates don't count as the special member functions that copy or move objects.
 * This effect can be good or bad:
 * - It can happend that a template constructor or assignment operator is a better match than the predefined
 *   copy/move constructor or assignment operator, although a template version is provided for initialization
 *   of other types only.
 * - It is not easy to "templify" a copy/move constructor, for example, to be able to constrain its existence.
 */

int main()
{
    // example 1: member templates
    Stack<int> int_stack;
    int_stack.push(10);
    Stack<float> float_stack;
    //...
    float_stack = int_stack;
    std::cout << float_stack.top() << std::endl;
    
    // example 2: specialization of member function templates
    std::cout << std::boolalpha;
    BoolString str1("hello");
    std::cout << str1.get() << std::endl;       // print 'hello'
    std::cout << str1.get<bool>() << std::endl; // print 'false'
    BoolString str2("on");
    std::cout << str2.get<bool>() << std::endl; // print 'true'
}