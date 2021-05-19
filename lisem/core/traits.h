#ifndef TRAITS_H
#define TRAITS_H




#include <utility>
#include "iostream"
#include <vector>
#include <functional>
#include <type_traits>
//===========================================================================
// type analysis

template<typename T>
struct is_pointer { static const bool value = false; };

template<typename T>
struct is_pointer<T*> { static const bool value = true; };

template <typename Fun>
struct is_fun_ptr
    : std::integral_constant<bool, std::is_pointer<Fun>::value
                            && std::is_function<
                                   typename std::remove_pointer<Fun>::type
                               >::value>
{
};


//specialization detection
template<typename Test, template<typename...> class Ref>
struct is_specialization : std::false_type {};

template<template<typename...> class Ref, typename... Args>
struct is_specialization<Ref<Args...>, Ref>: std::true_type {};

template<typename>
struct is_std_vector : std::false_type {};

template<typename T, typename A>
struct is_std_vector<std::vector<T,A>> : std::true_type
{

    typedef  T sub_type;
};




//===========================================================================
// META-FUNCTIONS FOR FUNCTION POINTER ANALYSIS

template<class F>
struct function_traits;

// function pointer
template<class R, class... Args>
struct function_traits<R(*)(Args...)> : public function_traits<R(Args...)>
{};

template<class R, class... Args>
struct function_traits<R(Args...)>
{
    using return_type = R;

    static constexpr std::size_t arity = sizeof...(Args);

    template <std::size_t N>
    struct argument
    {
        static_assert(N < arity, "error: invalid parameter index.");
        using type = typename std::tuple_element<N,std::tuple<Args...>>::type;
    };
};




template<typename F> struct FunctionTraits;

template<typename R, typename... Args>
struct FunctionTraits<R(*)(Args...)>
{
    using Pointer = R(*)(Args...);
    using RetType = R;
    using ArgTypes = std::tuple<Args...>;
    static constexpr std::size_t ArgCount = sizeof...(Args);
    template<std::size_t N>
    using NthArg = std::tuple_element_t<N, ArgTypes>;
    using FirstArg = NthArg<0>;
    using LastArg = NthArg<ArgCount - 1>;


};

template<typename R>
struct FunctionTraits<R(*)()>
{
    using Pointer = R(*)();
    using RetType = R;
    using ArgTypes = std::tuple<>;
    static constexpr std::size_t ArgCount = 0;
};



//===========================================================================
// META-FUNCTIONS FOR CREATING INDEX LISTS

// The structure that encapsulates index lists
template <size_t... Is>
struct index_list
{
};

// Collects internal details for generating index ranges [MIN, MAX)
namespace detail
{
    // Declare primary template for index range builder
    template <size_t MIN, size_t N, size_t... Is>
    struct range_builder;

    // Base step
    template <size_t MIN, size_t... Is>
    struct range_builder<MIN, MIN, Is...>
    {
        typedef index_list<Is...> type;
    };

    // Induction step
    template <size_t MIN, size_t N, size_t... Is>
    struct range_builder : public range_builder<MIN, N - 1, N - 1, Is...>
    {
    };
}

// Meta-function that returns a [MIN, MAX) index range
template<size_t MIN, size_t MAX>
using index_range = typename detail::range_builder<MIN, MAX>::type;


#define FUNC_N_ARG(f) (FunctionTraits<decltype(&f)>::ArgCount)







#endif // TRAITS_H
