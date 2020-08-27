/**
 * @file FunctionTraits.hpp
 * Function Traits
 *
 * Some supplementary function traits to STL type traits.
 */

#pragma once

#include "FTC/Utility/Lift.hpp"
#include "FTC/Utility/Variadic.hpp"

#include <cstddef>      // for std::size_t
#include <type_traits>  // for std::is_function, std::is_same, std::enable_if

namespace ftc {

/// @defgroup FunctionTraits Function Traits
/// @{

/// Checks if a type is a callable type (has operator()).
/// Types like std::function, lambdas, functors, native function are callables.
/// @note Overloaded callable type can not be detected with this. Use std::is_is_invocable instead.
template <typename F> struct is_callable;
template <typename F> inline constexpr bool is_callable_v = is_callable<F>::value;

/// Gets the return type of a callable type
/// @note The callable type must not be overloaded, otherwise use std::invoke_result instead.
template <typename F> struct result_of;
template <typename F> using result_of_t = typename result_of<F>::type;

/// Gets the argument count of a callable type
template <typename F> struct arity_of;
template <typename F> inline constexpr std::size_t arity_of_v = arity_of<F>::value;

/// Gets Nth argument type of a callable type
template <typename F, std::size_t N> struct arg_at;
template <typename F, std::size_t N> using arg_at_t = typename arg_at<F, N>::type;

/// Gets the class type of a member function pointer type
template <typename F> struct class_of_member_func;
template <typename F> using class_of_member_func_t = typename class_of_member_func<F>::type;

/// Gets the argument count of a callable
template <typename F> constexpr std::size_t ArityOf(F &&f);

/// Gets the invoked callable type of a overload set using Args as argument types
///
/// @note Needs C++20 support
#define INVOKE_FUNC_T(f, ...) std::invoke_result_t<decltype(LIFT(f)), __VA_ARGS__>

/// @}

}  // namespace ftc

///////////////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////////////

namespace ftc {

namespace detail {

    template <typename F, typename = void> struct IsCallable : std::is_function<F>
    {};

    template <typename F>
    struct IsCallable<F, std::enable_if_t<std::is_same_v<decltype(void(&F::operator())), void>>>
        : std::true_type
    {};

    // All callable types, eg. std::function, lambdas, functors
    template <typename F> struct FunctionTraits
    {
    private:
        using CallType = FunctionTraits<decltype(&F::operator())>;

    public:
        using RetType                         = typename CallType::RetType;
        static constexpr std::size_t ArgCount = CallType::ArgCount - 1;

        template <std::size_t N> struct Argument
        {
            static_assert(N < ArgCount, "FunctionTraits: Argument index out of range");
            using type = typename CallType::template Argument<N + 1>::type;
        };
    };

    template <typename Ret, typename... Args> struct FunctionTraits<Ret(Args...)>
    {
        using RetType                         = Ret;
        static constexpr std::size_t ArgCount = sizeof...(Args);

        template <std::size_t N> struct Argument
        {
            static_assert(N < ArgCount, "FunctionTraits: Argument index out of range");
            using type = typename variadic::type_at_t<N, Args...>;
        };
    };

    // Noexcept function
    template <typename Ret, typename... Args>
    struct FunctionTraits<Ret(Args...) noexcept> : FunctionTraits<Ret(Args...)>
    {};

    // L-value
    template <typename F> struct FunctionTraits<F &> : FunctionTraits<F>
    {};

    // R-value
    template <typename F> struct FunctionTraits<F &&> : FunctionTraits<F>
    {};

    // Pointer
    template <typename F> struct FunctionTraits<F *> : FunctionTraits<F>
    {};

    // 0. Member function pointer
    template <typename Class, typename Ret, typename... Args>
    struct FunctionTraits<Ret (Class::*)(Args...)> : FunctionTraits<Ret(Class &, Args...)>
    {
        using ClassType = Class;
    };

    // 1. Const member function pointer
    template <typename Class, typename Ret, typename... Args>
    struct FunctionTraits<Ret (Class::*)(Args...) const> : FunctionTraits<Ret (Class::*)(Args...)>
    {};

    // 2. Volatile member function pointer
    template <typename Class, typename Ret, typename... Args>
    struct FunctionTraits<Ret (Class::*)(Args...) volatile>
        : FunctionTraits<Ret (Class::*)(Args...)>
    {};

    // 3. Const volatile member function pointer
    template <typename Class, typename Ret, typename... Args>
    struct FunctionTraits<Ret (Class::*)(Args...) const volatile>
        : FunctionTraits<Ret (Class::*)(Args...)>
    {};

    // 4. Noexcept function pointer
    template <typename Class, typename Ret, typename... Args>
    struct FunctionTraits<Ret (Class::*)(Args...) noexcept>
        : FunctionTraits<Ret (Class::*)(Args...)>
    {};

    // 5. Const noexcept member function pointer
    template <typename Class, typename Ret, typename... Args>
    struct FunctionTraits<Ret (Class::*)(Args...) const noexcept>
        : FunctionTraits<Ret (Class::*)(Args...)>
    {};

    // 6. Volatile noexcept member function pointer
    template <typename Class, typename Ret, typename... Args>
    struct FunctionTraits<Ret (Class::*)(Args...) volatile noexcept>
        : FunctionTraits<Ret (Class::*)(Args...)>
    {};

    // 7. Const volatile noexcept member function pointer
    template <typename Class, typename Ret, typename... Args>
    struct FunctionTraits<Ret (Class::*)(Args...) const volatile noexcept>
        : FunctionTraits<Ret (Class::*)(Args...)>
    {};

}  // namespace detail

template <typename F>
struct is_callable : detail::IsCallable<std::remove_pointer_t<std::remove_reference_t<F>>>
{};

template <typename F> struct result_of
{
    static_assert(
        is_callable_v<F> || std::is_member_function_pointer_v<F>,
        "result_of: Type must be an unoverloaded callable type or a member function pointer");
    using type = typename detail::FunctionTraits<F>::RetType;
};

template <typename F> struct arity_of
{
    static_assert(
        is_callable_v<F> || std::is_member_function_pointer_v<F>,
        "arity_of: Type must be an unoverloaded callable type or a member function pointer");
    static constexpr std::size_t value = detail::FunctionTraits<F>::ArgCount;
};

template <typename F, std::size_t N> struct arg_at
{
    static_assert(
        is_callable_v<F> || std::is_member_function_pointer_v<F>,
        "arg_at: Type must be an unoverloaded callable type or a member function pointer");
    static_assert(N < arity_of_v<F>, "arg_at: Argument index out of range");
    using type = typename detail::FunctionTraits<F>::template Argument<N>::type;
};

template <typename F> struct class_of_member_func
{
    static_assert(std::is_member_function_pointer_v<F>,
                  "class_of_member_func: Must be a member function pointer");
    using type = typename detail::FunctionTraits<F>::ClassType;
};

template <typename F> constexpr std::size_t ArityOf(F &&f)
{
    return arity_of_v<decltype(std::forward<F>(f))>;
}

}  // namespace ftc
