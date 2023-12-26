#pragma once
#include <type_traits>

template<typename... Elements>
class Typelist {};

// Front: extracts the first element from the typelist
template<typename List>
class FrontT;

template<typename Head, typename... Tail>
class FrontT<Typelist<Head, Tail...>>
{
public:
    using Type = Head;
};
template<typename List>
using Front = typename FrontT<List>::Type;

// PopFront: removes the first element from the typelist
template<typename List>
class PopFrontT;

template<typename Head, typename... Tail>
class PopFrontT<Typelist<Head, Tail...>>
{
public:
    using Type = Typelist<Tail...>;
};
template<typename List>
using PopFront = typename PopFrontT<List>::Type;

// PushFront: insert elements onto the front of the typelist
template<typename List, typename NewElement>
class PushFrontT;

template<typename... Elements, typename NewElement>
class PushFrontT<Typelist<Elements...>, NewElement>
{
public:
    using Type = Typelist<NewElement, Elements...>;
};
template<typename List, typename NewElement>
using PushFront = typename PushFrontT<List, NewElement>::Type;

/**
 * Typelist Algorithms
 */
template<typename List>
class IsEmpty
{
public:
    static constexpr bool value = false;
};
template<>
class IsEmpty<Typelist<>> {
public:
    static constexpr bool value = true;
};

// NthElement: extracts the Nth element
template<typename List, unsigned N>
class NthElementT : public NthElementT<PopFront<List>, N-1> {};

template<typename List>
class NthElementT<List, 0> : public FrontT<List> {};

template<typename List, unsigned N>
using NthElement = typename NthElementT<List, N>::Type;

// Finding the match (LargestType)
template<typename List, bool = IsEmpty<List>::value>
class LargestTypeT;

template<typename List>
class LargestTypeT<List, false>
{
private:
    using First = Front<List>;
    using Rest = typename LargestTypeT<PopFront<List>>::Type;
public:
    using Type = std::conditional_t<(sizeof(First) >= sizeof(Rest)), First, Rest>;
};
template<typename List>
class LargestTypeT<List, true>
{
public:
    using Type = char;
};
template<typename List>
using LargestType = typename LargestTypeT<List>::Type;

// Appending to a typelist (PushBack)
template<typename List, typename NewElement>
class PushBackT;

template<typename... Elements, typename NewElement>
class PushBackT<Typelist<Elements...>, NewElement>
{
public:
    using Type = Typelist<Elements..., NewElement>;
};
template<typename List, typename NewElement>
using PushBack = typename PushBackT<List, NewElement>::Type;

// Reversing
template<typename List, bool = IsEmpty<List>::value>
class ReverseT;
template<typename List>
using Reverse = typename ReverseT<List>::Type;

template<typename List>
class ReverseT<List, false>
: public PushBackT<Reverse<PopFront<List>>, Front<List>> {};

template<typename List>
class ReverseT<List, true>
{
public:
    using Type = List;
};

// PopBack using Reverse
template<typename List>
class PopBackT
{
public:
    using Type = Reverse<PopFront<Reverse<List>>>;
};
template<typename List>
using PopBack = typename PopBackT<List>::Type;

// Transforming

template<typename List, template<typename T> typename MetaFun, bool = IsEmpty<List>::value>
class TransformT;

template<typename... Elements, template<typename T> typename MetaFun>
class TransformT<Typelist<Elements...>, MetaFun, false>
{
public:
    using Type = Typelist<typename MetaFun<Elements>::Type...>;
};

template<typename List, template<typename T> typename MetaFun>
class TransformT<List, MetaFun, true>
{
public:
    using Type = List;
};

template<typename List, template<typename T> typename MetaFun>
using Transform = typename TransformT<List, MetaFun>::Type;

// Accumulating
template<typename List,
         template<typename X, typename Y> typename F,
         typename I,
         bool = IsEmpty<List>::value>
class AccumulateT;

template<typename List,
         template<typename X, typename Y> typename F,
         typename I>
class AccumulateT<List, F, I, false>
: public AccumulateT<PopFront<List>, F, typename F<I, Front<List>>::Type> {};

template<typename List,
         template<typename X, typename Y> typename F,
         typename I>
class AccumulateT<List, F, I, true>
{
public:
    using Type = I;
};
template<typename List,
         template<typename X, typename Y> typename F,
         typename I>
using Accumulate = typename AccumulateT<List, F, I>::Type;

// Insertion Sort
template<typename T>
struct IdentityT {
    using Type = T;
};
template<typename T>
using Identity = typename IdentityT<T>::Type;

template<typename List, typename Element,
         template<typename T, typename U> class Compare,
         bool = IsEmpty<List>::value>
class InsertSortedT;

template<typename List, typename Element,
         template<typename T, typename U> class Compare>
class InsertSortedT<List, Element, Compare, false>
{
    // compute the tail of the resuling list
    using NewTail = std::conditional_t<Compare<Element, Front<List>>::value,
                                       Identity<List>,
                                       typename InsertSortedT<PopFront<List>, Element, Compare>::Type>;
    // compute the head of the resuling list
    using NewHead = std::conditional_t<Compare<Element, Front<List>>::value,
                                       Element,
                                       Front<List>>;
public:
    using Type = PushFront<NewTail, NewHead>;
};

template<typename List, typename Element,
         template<typename T, typename U> class Compare>
class InsertSortedT<List, Element, Compare, true>
: public PushFrontT<List, Element> {};
template<typename List, typename Element,
         template<typename T, typename U> class Compare>
using InsertSorted = typename InsertSortedT<List, Element, Compare>::Type;

template<typename List,
         template<typename T, typename U> class Compare,
         bool = IsEmpty<List>::value>
class InsertionSortT;

template<typename List,
         template<typename T, typename U> class Compare>
using InsertionSort = typename InsertionSortT<List, Compare>::Type;

template<typename List,
         template<typename T, typename U> class Compare>
class InsertionSortT<List, Compare, false>
: public InsertSortedT<InsertionSort<PopFront<List>, Compare>, Front<List>, Compare> {};

template<typename List,
         template<typename T, typename U> class Compare>
class InsertionSortT<List, Compare, true>
{
public:
    using Type = List;
};