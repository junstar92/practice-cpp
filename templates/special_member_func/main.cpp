/**
 * Member function templates can also be used as special member functions, including as a constructor,
 * which, however, might lead to surprising behavior
 */
#include <iostream>
#include <string>
#include <utility>

namespace v1 {

class Person
{
private:
    std::string name;
public:
    // constructor for passed initial name
    explicit Person(std::string const& n) : name{n} {
        std::cout << "copying string-CONSTR for '" << name << "'\n";
    }
    explicit Person(std::string&& n) : name{std::move(n)} {
        std::cout << "moving string-CONSTR for'" << name << "'\n";
    }
    // copy and move constructor
    Person(Person const& p) : name{p.name} {
        std::cout << "COPY-CONSTR Person '" << name << "'\n";
    }
    Person(Person&& p) : name{std::move(p.name)} {
        std::cout << "MOVE-CONSTR Person '" << name << "'\n";
    }
};

}

namespace v2 {
/**
 * Now, let's replace the two string constructors with one generic constructor perfect forwarding
 * the passed argument to the member name.
 */
class Person
{
private:
    std::string name;
public:
    // generic constructor for passed initial name
    template<typename STR>
    explicit Person(STR&& n) : name{std::forward<STR>(n)} {
        std::cout << "TMPL-CONSTR for '" << name << "'\n";
    }
    // copy and move constructor
    Person(Person const& p) : name{p.name} {
        std::cout << "COPY-CONSTR Person '" << name << "'\n";
    }
    Person(Person&& p) : name{std::move(p.name)} {
        std::cout << "MOVE-CONSTR Person '" << name << "'\n";
    }
};

}


namespace v3 {
#include <type_traits>

// for C++17
template<typename T>
using EnableIfString = std::enable_if_t<std::is_convertible_v<T, std::string>>;
// for C++14
template<typename T>
using EnableIfStringCPP14 = std::enable_if_t<std::is_convertible<T, std::string>::value>;
// for C++11
template<typename T>
using EnableIfStringCPP11 = typename std::enable_if<std::is_convertible<T, std::string>::value>::type;

class Person
{
private:
    std::string name;
public:
    // generic constructor for passed initial name
    template<typename STR, typename = EnableIfString<STR>>
    explicit Person(STR&& n) : name{std::forward<STR>(n)} {
        std::cout << "TMPL-CONSTR for '" << name << "'\n";
    }
    // copy and move constructor
    Person(Person const& p) : name{p.name} {
        std::cout << "COPY-CONSTR Person '" << name << "'\n";
    }
    Person(Person&& p) : name{std::move(p.name)} {
        std::cout << "MOVE-CONSTR Person '" << name << "'\n";
    }
};

}

class C1
{
public:
    C1() {}
    template<typename T>
    C1(T const&) {
        std::cout << "tmpl copy constructor\n";
    }
};

class C2
{
public:
    C2() {}
    // user-define the predefined copy constructor as deleted
    // (with conversion to volatile to enable better matches)
    C2(C2 const volatile&) = delete;
    // implement copy constructor template with better match:
    template<typename T>
    C2(T const&) {
        std::cout << "tmpl copy constructor\n";
    }
};

int main()
{
    std::cout << "----------------------------\n";
    {
        /**
         * 
         */
        std::string s = "sname";
        v1::Person p1(s);               // init with string object => calls copying string-CONSTR
        v1::Person p2("tmp");           // init with string literal => calls moving string-CONSTR
        v1::Person p3(p1);              // copy Person => calls COPY-CONSTR
        v1::Person p4(std::move(p1));   // move Person => calls MOVE-CONSTR
    }
    std::cout << "----------------------------\n";
    {
        std::string s = "sname";
        v2::Person p1(s);               // init with string object => calls copying TMPL-CONSTR
        v2::Person p2("tmp");           // init with string literal => calls moving TMPL-CONSTR
        // v2::Person p3(p1);           // **ERROR**
        v2::Person p4(std::move(p1));   // OK: move Person => calls MOVE-CONSTR
        // Note that also copying a constant Person works fine:
        v2::Person const p2c("ctmp");   // init constant object with string literal (calls TMPL-CONSTR)
        v2::Person p3c(p2c);            // OK: copy constant Person => calls COPY-CONSTR
        /**
         * The problem of `v2::Person p3(p1);` is that, according to the overload resolution rules of C++,
         * for nonconstant lvalue Person p the member template ('Person(STR&& n)') is a better match than
         * the (usually predefined) copy constructor: ('Person(Person const& p)`). `STR` is just subsituted
         * with `Person&`, while for the copy constructor a conversion to const is necessary.
         * 
         * You might think about solving this by also providing a nonconstant copy constructor: `Person(Person& p)`.
         * However, that is only a partial solution because for objects of a derived class, the member template is
         * still a better match. What you really want is to disable the member template for the case that the passed
         * argument is a Person or an expresion that can be converted to a Person. This can be done by using `std::enable_if<>,
         * which is introduced in the `v3` namespace
         */
    }
    std::cout << "----------------------------\n";
    {
        std::string s = "sname";
        v3::Person p1(s);               // init with string object => calls copying TMPL-CONSTR
        v3::Person p2("tmp");           // init with string literal => calls moving TMPL-CONSTR
        v3::Person p3(p1);              // copy Person => calls COPY-CONSTR
        v3::Person p4(std::move(p1));   // move Person => calls MOVE-CONSTR
    }
    std::cout << "----------------------------\n";
    /**
     * Note that normally we can't use enable_if<> to disable the predefined copy/move constructors and/or
     * assignment operators. The reason is that member function templates never count as special member functions
     * and are ignored when, for example, a copy constructor is needed.
     */
    {
        // for example:
        C1 x;
        C1 y{x}; // still uses the predefined copy constructor (not the member template)
    }
    /**
     * Deleting the predefined copy constructor is no solution, because then the trial to copy a C1 results in
     * an error !
     * There is a tricky solution: We can declare a copy constructor for `const volatile` arguments and mark it
     * "deleted" (i.e., define it with `=delete`). Doing so prevents another copy constructor from being implicitly
     * declared. With that in place, we can define a constructor template that will be preferred over the (deleted)
     * copy constructor for nonvolatile types. Refer to C2 class defined.
     */
    {
        C2 x;
        C2 y{x}; // uses the member template
    }
}