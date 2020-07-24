#pragma once
#include "FTC/Utility/Lift.hpp"
#include "FTC/Utility/Variadic.hpp"

#include <cstddef>  // for std::size_t

namespace ftc {

/// Get the return type of a callable type
template <typename F> struct result_of;
template <typename F> using result_of_t = typename result_of<F>::type;

/// Get the argument count of a callable type
template <typename F> struct arity_of;
template <typename F> inline constexpr std::size_t arity_of_v = arity_of<F>::value;

/// Get Nth argument type of a callable type
template <typename F, std::size_t N> struct arg_at;
template <typename F, std::size_t N> using arg_at_t = typename arg_at<F, N>::type;

/// Get the class type of a member function pointer type
template <typename F> struct class_of_member_func;
template <typename F> using class_of_member_func_t = typename class_of_member_func<F>::type;

/// Get the argument count of a callable
template <typename F> constexpr std::size_t ArityOf(F &&f);

/// Get the invoked callable type of a overload set using Args as argument types
///
/// @note Needs C++20 support
#define INVOKE_FUNC_T(f, ...) std::invoke_result_t<decltype(LIFT(f)), __VA_ARGS__>

}  // namespace ftc


///////////////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////////////

namespace ftc {

namespace detail {
    // All callable type, eg. std::function, functor
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

    // Member function pointer
    template <typename Class, typename Ret, typename... Args>
    struct FunctionTraits<Ret (Class::*)(Args...)> : FunctionTraits<Ret(Class &, Args...)>
    {
        using ClassType = Class;
    };

    // Const member function pointer
    template <typename Class, typename Ret, typename... Args>
    struct FunctionTraits<Ret (Class::*)(Args...) const> : FunctionTraits<Ret (Class::*)(Args...)>
    {};

    // Noexcept function pointer
    template <typename Class, typename Ret, typename... Args>
    struct FunctionTraits<Ret (Class::*)(Args...) noexcept>
        : FunctionTraits<Ret (Class::*)(Args...)>
    {};

    // Const noexcept member function pointer
    template <typename Class, typename Ret, typename... Args>
    struct FunctionTraits<Ret (Class::*)(Args...) const noexcept>
        : FunctionTraits<Ret (Class::*)(Args...)>
    {};

}  // namespace detail

template <typename F> struct result_of
{
    using type = typename detail::FunctionTraits<F>::RetType;
};

template <typename F> struct arity_of
{
    static constexpr std::size_t value = detail::FunctionTraits<F>::ArgCount;
};

template <typename F, std::size_t N> struct arg_at
{
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
