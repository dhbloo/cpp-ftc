#pragma once
#include <cstddef>  // for std::size_t
#include <tuple>    // for std::make_tuple & std::apply
#include <utility>  // for std::forward

namespace cpppg::variadic {

/// Check if parameter pack is empty
template <typename... Ts> constexpr bool IsEmpty(Ts &&... vs);

/// Return the length of a parameter pack
template <typename... Ts> constexpr auto Length(Ts &&... vs);

/// Return the head element of a parameter list
template <typename... Ts> constexpr auto Head(Ts &&... vs);

/// Return the last element of a parameter list
template <typename... Ts> constexpr auto Last(Ts &&... vs);

/// Return the n-th element of a parameter list
///
/// Negative N refers to reversed list, eg. -1 gives the last parameter,
/// -2 gaives penultimate parameter.
///
/// @tparam N Index of element, should be in range [-Length, Length - 1]
template <int Idx, typename... Ts> constexpr auto Get(Ts &&... vs);

/// Return f(xn,...,f(x2, f(x1, init))...) or init if parameter list is empty.
template <typename F, typename Acc, typename... Ts>
constexpr auto Foldl(F &&func, Acc &&init, Ts &&... vs);

/// Return f(x1, f(x2, ..., f(xn, init)...)) or init if parameter list is empty.
template <typename F, typename Acc, typename... Ts>
constexpr auto Foldr(F &&func, Acc &&init, Ts &&... vs);

/// Returns true if any element in parameter list has f(x) evaluates to true.
template <typename F, typename... Ts> constexpr bool Exists(F &&func, Ts &&... vs);

/// Returns true if all elements in parameter list has f(x) evaluates to true.
template <typename F, typename... Ts> constexpr bool All(F &&func, Ts &&... vs);

/// Apply func to each of the element in the parameter list.
template <typename F, typename... Ts> constexpr auto ForEach(F &&func, Ts &&... vs);

/// Rotate left a parameter list
template <std::size_t N, typename... Ts> constexpr auto RotateLeft(Ts &&... vs);

/// Rotate right a parameter list
template <std::size_t N, typename... Ts> constexpr auto RotateRight(Ts &&... vs);

}  // namespace cpppg::variadic

///////////////////////////////////////////////////////////////////////////////

namespace cpppg::variadic {

namespace detail {

    template <typename T, typename... Ts> constexpr auto Head(T &&head, Ts &&... tail)
    {
        return std::forward<T>(head);
    }

    template <typename T> constexpr auto Last(T &&head) { return std::forward<T>(head); }
    template <typename T, typename... Ts> constexpr auto Last(T &&head, Ts &&... tail)
    {
        return Last(std::forward<Ts>(tail)...);
    }

    template <std::size_t N, typename T, typename... Ts> struct NthElement
    {
        static constexpr auto call(T &&head, Ts &&... tail)
        {
            return NthElement<N - 1, Ts...>::call(std::forward<Ts>(tail)...);
        }
    };

    template <typename T, typename... Ts> struct NthElement<0, T, Ts...>
    {
        static constexpr auto call(T &&head, Ts &&... tail) { return std::forward<T>(head); }
    };

    template <typename F, typename Acc, typename T>
    constexpr auto Foldl(F &&func, Acc &&init, T &&v)
    {
        return func(std::forward<Acc>(init), std::forward<T>(v));
    }

    template <typename F, typename Acc, typename T, typename... Ts>
    constexpr auto Foldl(F &&func, Acc &&init, T &&v, Ts &&... vs)
    {
        return Foldl(std::forward<F>(func),
                     func(std::forward<Acc>(init), std::forward<T>(v)),
                     std::forward<Ts>(vs)...);
    }

    template <typename F, typename Acc, typename T>
    constexpr auto Foldr(F &&func, Acc &&init, T &&v)
    {
        return func(std::forward<T>(v), std::forward<Acc>(init));
    }

    template <typename F, typename Acc, typename T, typename... Ts>
    constexpr auto Foldr(F &&func, Acc &&init, T &&v, Ts &&... vs)
    {
        return func(std::forward<T>(v),
                    Foldr(std::forward<F>(func), std::forward<Acc>(init), std::forward<Ts>(vs)...));
    }

    template <std::size_t N> struct RotateLeft
    {
        template <typename F, typename T, typename... Ts>
        static constexpr auto call(F &&func, T &&v, Ts &&... vs)
        {
            return RotateLeft<N - 1>::call(std::forward<F>(func),
                                           std::forward<Ts>(vs)...,
                                           std::forward<T>(v));
        }
    };

    template <> struct RotateLeft<0>
    {
        template <typename F, typename T, typename... Ts>
        static constexpr auto call(F &&func, T &&v, Ts &&... vs)
        {
            return func(std::forward<T>(v), std::forward<Ts>(vs)...);
        }
    };

}  // namespace detail

template <typename... Ts> constexpr bool IsEmpty(Ts &&... vs)
{
    return sizeof...(vs) == 0;
}

template <typename... Ts> constexpr auto Length(Ts &&... vs)
{
    return sizeof...(vs);
}

template <typename... Ts> constexpr auto Head(Ts &&... vs)
{
    static_assert(sizeof...(vs) > 0, "Empty variadic list");
    return detail::Head(std::forward<Ts>(vs)...);
}

template <typename... Ts> constexpr auto Last(Ts &&... vs)
{
    static_assert(sizeof...(vs) > 0, "Empty variadic list");
    return detail::Last(std::forward<Ts>(vs)...);
}

template <int Idx, typename... Ts> constexpr auto Get(Ts &&... vs)
{
    constexpr int Len = sizeof...(vs);
    static_assert(-Len <= Idx && Idx < Len, "Index out of range");
    return detail::NthElement<(Idx + Len) % Len, Ts...>::call(std::forward<Ts>(vs)...);
}

template <typename F, typename Acc, typename... Ts>
constexpr auto Foldl(F &&func, Acc &&init, Ts &&... vs)
{
    if constexpr (sizeof...(vs) == 0) {
        return std::forward<Acc>(init);
    }
    else {
        return detail::Foldl(std::forward<F>(func),
                             std::forward<Acc>(init),
                             std::forward<Ts>(vs)...);
    }
}

template <typename F, typename Acc, typename... Ts>
constexpr auto Foldr(F &&func, Acc &&init, Ts &&... vs)
{
    if constexpr (sizeof...(vs) == 0) {
        return std::forward<Acc>(init);
    }
    else {
        return detail::Foldr(std::forward<F>(func),
                             std::forward<Acc>(init),
                             std::forward<Ts>(vs)...);
    }
}

template <typename F, typename... Ts> constexpr bool Exists(F &&func, Ts &&... vs)
{
    return (func(std::forward<Ts>(vs)) || ...);
}

template <typename F, typename... Ts> constexpr bool All(F &&func, Ts &&... vs)
{
    return (func(std::forward<Ts>(vs)) && ...);
}

template <typename F, typename... Ts> constexpr auto ForEach(F &&func, Ts &&... vs)
{
    return (func(std::forward<Ts>(vs)), ...);
}

template <std::size_t N, typename... Ts> constexpr auto RotateLeft(Ts &&... vs)
{
    return [vs = std::make_tuple(std::forward<Ts>(vs)...)](auto &&func) -> auto
    {
        return std::apply(
            [func = std::forward<decltype(func)>(func)](auto &&... vs) {
                return detail::RotateLeft<N>::call(std::forward<decltype(func)>(func),
                                                   std::forward<decltype(vs)>(vs)...);
            },
            std::forward<decltype(vs)>(vs));
    };
}

template <std::size_t N, typename... Ts> constexpr auto RotateRight(Ts &&... vs)
{
    return [vs = std::make_tuple(std::forward<Ts>(vs)...)](auto &&func) -> auto
    {
        return std::apply(
            [func = std::forward<decltype(func)>(func)](auto &&... vs) {
                return detail::RotateLeft<sizeof...(vs) - N>::call(
                    std::forward<decltype(func)>(func),
                    std::forward<decltype(vs)>(vs)...);
            },
            std::forward<decltype(vs)>(vs));
    };
}

}  // namespace cpppg::variadic
