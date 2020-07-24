#pragma once
#include <cstddef>      // for std::size_t
#include <optional>     // for std::optional
#include <type_traits>  // for std::is_same
#include <utility>      // for std::forward, std::declval

namespace ftc::variadic {

/* -------------------------------------------------------------------------
   Variadic type traits & type alias
   ------------------------------------------------------------------------- */

/// Check if variadic types are same
template <typename... Ts> struct are_same;
template <typename... Ts> inline constexpr bool are_same_v = are_same<Ts...>::value;

/// Get the first type in a type list
template <typename... Ts> struct head;
template <typename... Ts> using head_t = typename head<Ts...>::type;

/// Get the last type in a type list
template <typename... Ts> struct last;
template <typename... Ts> using last_t = typename last<Ts...>::type;

/// Get type at Idx in a variadic type pack
template <std::size_t Idx, typename... Ts> struct type_at;
template <std::size_t Idx, typename... Ts> using type_at_t = typename type_at<Idx, Ts...>::type;

/* -------------------------------------------------------------------------
   Variadic arguments query functions
   ------------------------------------------------------------------------- */

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
/// Negative Idx refers to reversed list, eg. -1 gives the last parameter,
/// -2 gaives penultimate parameter.
///
/// @tparam Idx Index of element, should be in range [-Length, Length - 1].
template <int Idx, typename... Ts> constexpr auto Get(Ts &&... vs);

/* -------------------------------------------------------------------------
   Variadic arguments functor functions

   Functor function takes a functor and other arguments (if exists) and returns
   a functor that takes the actual variadic arguments.
   ------------------------------------------------------------------------- */

/// Return f(xn,...,f(x2, f(x1, init))...) or init if parameter list is empty.
///
/// @param func Reducer functor
/// @param init Initial value
/// @return A functor that takes the actual arguments.
template <typename F, typename Acc> constexpr auto Foldl(F &&func, Acc &&init);

/// Return f(xn,...,f(x3, f(x2, x1))...)
///
/// The variadic argument list should contain at least 2 elements.
///
/// @param func Reducer functor
/// @return A functor that takes the actual arguments.
template <typename F> constexpr auto Foldl(F &&func);

/// Return f(x1, f(x2, ..., f(xn, init)...)) or init if parameter list is empty.
///
/// @param func Reducer functor
/// @param init Initial value
/// @return A functor that takes the actual arguments.
template <typename F, typename Acc> constexpr auto Foldr(F &&func, Acc &&init);

/// Return f(x1, f(x2, ..., f(xn-1, xn)...)).
///
/// The variadic argument list should contain at least 2 elements.
///
/// @param func Reducer functor
/// @param init Initial value
/// @return A functor that takes the actual arguments.
template <typename F> constexpr auto Foldr(F &&func);

/// Returns true if any element in parameter list has f(x) evaluates to true.
///
/// @param func Boolean predicate
/// @return A functor that takes the actual arguments and returns boolean value.
template <typename F> constexpr auto Exists(F &&func);

/// Returns true if all elements in parameter list has f(x) evaluates to true.
///
/// @param func Boolean predicate
/// @return A functor that takes the actual arguments and returns boolean value.
template <typename F> constexpr auto All(F &&func);

/// Apply func to each of the element in the parameter list.
///
/// @param func Functor to apply
/// @return A functor that takes the actual arguments and returns void.
template <typename F> constexpr auto ForEach(F &&func);

/// Find the first element in an argument list that evaluates f(x) to true.
///
/// Applies f to each element x of the argument list, from left to right, until f(x)
/// evaluates to true. It returns optional{x} if such an x exists; otherwise it returns
/// none optional{}.
///
/// @note Variadic arguments must have homogeneous types, otherwise it will fail to compile.
///
/// @param func Predicate functor
/// @return A functor that takes the actual arguments and returns optional value.
template <typename F> constexpr auto FindFirst(F &&func);

/* -------------------------------------------------------------------------
   Variadic arguments transformation functions

   Transformation function is a high-order function that forward the transformed
   arguments to the provided functor.
   ------------------------------------------------------------------------- */

/// Reverse an argument list, then forward them to the given functor.
///
/// @param  funcToForward A functor to take the transformed arguments.
/// @return A functor that takes the actual arguments.
template <typename F> constexpr auto Reverse(F &&funcToForward);

/// Rotate left an argument list, then forward them to the given functor.
///
/// Cyclic left shift N arguments to the right. N must be not less than 0.
///
/// @tparam N Left shift amount, should be at least 0.
/// @param  funcToForward A functor to take the transformed arguments.
/// @return A functor that takes the actual arguments.
template <std::size_t N, typename F> constexpr auto RotateLeft(F &&funcToForward);

/// Rotate right an argument list, then forward them to the given functor.
///
/// Cyclic right shift N arguments to the right. N must be not less than 0.
///
/// @tparam N Right shift amount, should be at least 0.
/// @param  funcToForward A functor to take the transformed arguments.
/// @return A functor that takes the actual arguments.
template <std::size_t N, typename F> constexpr auto RotateRight(F &&funcToForward);

/// Rotate an argument list, then forward them to the given functor.
///
/// For non-negetive N, this is same as RotateLeft<N>. For negetive N, this is same as
/// RotateRight<-N>.
///
/// @tparam N Left shift amount, negetive value means right shift.
/// @param  funcToForward A functor to take the transformed arguments.
/// @return A functor that takes the actual arguments.
template <int N, typename F> constexpr auto Rotate(F &&funcToForward);

/// Swap two element in an argument list, then forward them to the given functor.
///
/// @tparam I Index of first element.
/// @tparam J Index of second element.
/// @param  funcToForward A functor to take the transformed arguments.
/// @return A functor that takes the actual arguments.
template <std::size_t I, std::size_t J, typename F> constexpr auto Swap(F &&funcToForward);

/// Take N arguments from the argument list, then forward them to the given functor.
///
/// For positive N, first N arguments are forwarded to the given functor. For negetive N,
/// last N arguments are forwarded to the given functor. N must be not greater than the
/// number of variadic arguments.
///
/// @tparam N How many arguments to take.
/// @param  funcToForward A functor to take the transformed arguments.
/// @return A functor that takes the actual arguments.
template <int N, typename F> constexpr auto Take(F &&funcToForward);

/// Drop N arguments from the argument list, then forward them to the given functor.
///
/// For positive N, all but first N arguments are forwarded to the given functor. For
/// negetive N, all but last N arguments are forwarded to the given functor. N must be
/// not greater than the number of variadic arguments.
///
/// @tparam N How many arguments to drop.
/// @param  funcToForward A functor to take the transformed arguments.
/// @return A functor that takes the actual arguments.
template <int N, typename F> constexpr auto Drop(F &&funcToForward);

/// Get arguments in range [N, M), then forward them to the given functor.
///
/// N must be less than M.
///
/// @tparam N Slice starting index.
/// @tparam M Slice ending index.
/// @param  funcToForward A functor to take the transformed arguments.
/// @return A functor that takes the actual arguments.
template <int N, int M, typename F> constexpr auto Slice(F &&funcToForward);

/// Applies mapper to every elements, from left to right, then forward them to the given functor.
///
/// @param  mapper A functor to transform every element.
/// @param  funcToForward A functor to take the mapped arguments.
/// @return A functor that takes the actual arguments.
template <typename Mapper, typename F> constexpr auto Map(Mapper &&mapper, F &&funcToForward);

/// Remove elements that evaluate the predicate to false in an argument list, then forward them
/// to the given functor.
///
/// Applies predicate to each element x of the argument list, from left to right, and forwards the
/// list of those x for which pred(x) evaluated to true, in the same order as they occurred in the
/// argument list.
///
/// @param  pred Predicate functor that returns boolean.
/// @param  funcToForward A functor to take the mapped arguments.
/// @return A functor that takes the actual arguments.
template <typename Pred, typename F> constexpr auto Filter(Pred &&pred, F &&funcToForward);

/// Replace element at Idx in the argument list to the given value, then forward them to the
/// given functor.
///
/// For non-negetive index, the argument at index counted from beginning is replaced. For
/// negetive index, the argument at index counted from back is replaced. Index must not be
/// outside the range of variadic arguments.
///
/// @tparam Idx Index of element to replace, should be in range [-Length, Length - 1].
/// @param  value Replacement value
/// @param  funcToForward A functor to take the mapped arguments.
/// @return A functor that takes the actual arguments.
template <int Idx, typename Val, typename F>
constexpr auto ReplaceAt(Val &&value, F &&funcToForward);

/// Permute an argument list using given index, then forward them to the given functor.
///
/// @note Each index must be inside the range of argument list. Negetive index refers to
/// the element counted from back.
/// 
/// @tparam Idx... A sequence of indices to permute argument list.
/// @param  funcToForward A functor to take the mapped arguments.
/// @return A functor that takes the actual arguments.
template <int... Idx, typename F> constexpr auto Permute(F &&funcToForward);

/* -------------------------------------------------------------------------
   Variadic arguments generator functions
   ------------------------------------------------------------------------- */

/// Generate a variadic index sequence, then forward them to the given functor.
///
/// Index sequence 0, 1, 2, ..., N-1 will be generated as variadic argument list.
///
/// @tparam N Number of indices
/// @param  funcToForward A functor to take the transformed arguments.
template <std::size_t N, typename F> constexpr auto IndexSequence(F &&funcToForward);

/// Generate a variadic int sequence, then forward them to the given functor.
///
/// IntSequence generates a similar sequence to what for-loop generates. Given beginning
/// index, ending index and step, the generated sequence is identical to this for-loop:
///     for (int i = Begin; Step > 0 ? i < End : i > End; i += Step) { ... }
/// @note Step must not be zero and range must be vaild, otherwise it will fail to compile.
///
/// @tparam Begin Beginning index
/// @tparam End   Ending index
/// @tparam Step  Step value, default is 1
/// @param  funcToForward A functor to take the transformed arguments.
template <int Begin, int End, int Step = 1, typename F>
constexpr auto IntSequence(F &&funcToForward);

}  // namespace ftc::variadic


///////////////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////////////

namespace ftc::variadic {

namespace detail {

    template <typename... Ts> struct are_same;

    template <typename First, typename Second, typename... Ts> struct are_same<First, Second, Ts...>
    {
        static bool const value = std::is_same_v<First, Second> && are_same<Second, Ts...>::value;
    };

    template <typename T> struct are_same<T> : std::true_type
    {};

    template <> struct are_same<> : std::true_type
    {};

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

    template <typename F, typename T, typename U> constexpr auto Foldl(F &&func, T &&v1, U &&v2)
    {
        return std::forward<F>(func)(std::forward<T>(v1), std::forward<U>(v2));
    }

    template <typename F, typename T, typename U, typename... Ts>
    constexpr auto Foldl(F &&func, T &&v1, U &&v2, Ts &&... vs)
    {
        return Foldl(std::forward<F>(func),
                     std::forward<F>(func)(std::forward<T>(v1), std::forward<U>(v2)),
                     std::forward<Ts>(vs)...);
    }

    template <typename F, typename T, typename U> constexpr auto Foldr(F &&func, T &&v1, U &&v2)
    {
        return std::forward<F>(func)(std::forward<T>(v1), std::forward<U>(v2));
    }

    template <typename F, typename T, typename U, typename... Ts>
    constexpr auto Foldr(F &&func, T &&v1, U &&v2, Ts &&... vs)
    {
        return std::forward<F>(func)(
            std::forward<T>(v1),
            Foldr(std::forward<F>(func), std::forward<U>(v2), std::forward<Ts>(vs)...));
    }

    template <std::size_t N, typename... Revs> struct Reverse
    {
        template <typename F, typename T, typename... Ts>
        static constexpr auto call(F &&func, Revs &&... revs, T &&v, Ts &&... vs)
        {
            return Reverse<N - 1, T, Revs...>::call(std::forward<F>(func),
                                                    std::forward<T>(v),
                                                    std::forward<Revs>(revs)...,
                                                    std::forward<Ts>(vs)...);
        }
    };

    template <typename... Ts> struct Reverse<0, Ts...>
    {
        template <typename F> static constexpr auto call(F &&func, Ts &&... vs)
        {
            return std::forward<F>(func)(std::forward<Ts>(vs)...);
        }
    };

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
        template <typename F, typename... Ts> static constexpr auto call(F &&func, Ts &&... vs)
        {
            return std::forward<F>(func)(std::forward<Ts>(vs)...);
        }
    };

    template <std::size_t I, std::size_t J, std::size_t N> struct Swap
    {
        template <typename F, typename IV, typename JV, typename T, typename... Ts>
        static constexpr auto call(F &&func, IV &&iv, JV &&jv, T &&v, Ts &&... vs)
        {
            if constexpr (N == I) {
                return Swap<I, J, N - 1>::call(std::forward<F>(func),
                                               std::forward<IV>(iv),
                                               std::forward<JV>(jv),
                                               std::forward<Ts>(vs)...,
                                               std::forward<JV>(jv));
            }
            else if constexpr (N == J) {
                return Swap<I, J, N - 1>::call(std::forward<F>(func),
                                               std::forward<IV>(iv),
                                               std::forward<JV>(jv),
                                               std::forward<Ts>(vs)...,
                                               std::forward<IV>(iv));
            }
            else {
                return Swap<I, J, N - 1>::call(std::forward<F>(func),
                                               std::forward<IV>(iv),
                                               std::forward<JV>(jv),
                                               std::forward<Ts>(vs)...,
                                               std::forward<T>(v));
            }
        }
    };

    template <std::size_t I, std::size_t J> struct Swap<I, J, 0>
    {
        template <typename F, typename IV, typename JV, typename... Ts>
        static constexpr auto call(F &&func, IV &&iv, JV &&jv, Ts &&... vs)
        {
            return std::forward<F>(func)(std::forward<Ts>(vs)...);
        }
    };

    template <std::size_t I, std::size_t N> struct Swap<I, I, N>
    {
        template <typename F, typename IV, typename JV, typename... Ts>
        static constexpr auto call(F &&func, IV &&iv, JV &&jv, Ts &&... vs)
        {
            return std::forward<F>(func)(std::forward<Ts>(vs)...);
        }
    };

    template <std::size_t N> struct DropFirst
    {
        template <typename F, typename T, typename... Ts>
        static constexpr auto call(F &&func, T &&v, Ts &&... vs)
        {
            return DropFirst<N - 1>::call(std::forward<F>(func), std::forward<Ts>(vs)...);
        }
    };

    template <> struct DropFirst<0>
    {
        template <typename F, typename... Ts> static constexpr auto call(F &&func, Ts &&... vs)
        {
            return std::forward<F>(func)(std::forward<Ts>(vs)...);
        }
    };

    template <std::size_t N> struct Filter
    {
        template <typename F, typename Pred, typename T, typename... Ts>
        static constexpr auto call(F &&func, Pred &&pred, T &&v, Ts &&... vs)
        {
            if (std::forward<Pred>(pred)(std::forward<T>(v))) {
                return Filter<N - 1>::call(std::forward<F>(func),
                                           std::forward<Pred>(pred),
                                           std::forward<Ts>(vs)...,
                                           std::forward<T>(v));
            }
            else {
                return Filter<N - 1>::call(std::forward<F>(func),
                                           std::forward<Pred>(pred),
                                           std::forward<Ts>(vs)...);
            }
        }
    };

    template <> struct Filter<0>
    {
        template <typename F, typename Pred, typename... Ts>
        static constexpr auto call(F &&func, Pred &&pred, Ts &&... vs)
        {
            return std::forward<F>(func)(std::forward<Ts>(vs)...);
        }
    };

    template <std::size_t I, std::size_t N> struct ReplaceAt
    {
        template <typename F, typename Val, typename T, typename... Ts>
        static constexpr auto call(F &&func, Val &&val, T &&v, Ts &&... vs)
        {
            if constexpr (I == N) {
                return RotateLeft<N - 1>::call(std::forward<F>(func),
                                               std::forward<Ts>(vs)...,
                                               std::forward<Val>(val));
            }
            else {
                return ReplaceAt<I, N - 1>::call(std::forward<F>(func),
                                                 std::forward<Val>(val),
                                                 std::forward<Ts>(vs)...,
                                                 std::forward<T>(v));
            }
        }
    };

    template <std::size_t N> struct GenIndexSequence
    {
        template <typename F, typename... Ts> static constexpr auto call(F &&func, Ts &&... vs)
        {
            return GenIndexSequence<N - 1>::call(std::forward<F>(func),
                                                 N - 1,
                                                 std::forward<Ts>(vs)...);
        }
    };

    template <> struct GenIndexSequence<0>
    {
        template <typename F, typename... Ts> static constexpr auto call(F &&func, Ts &&... vs)
        {
            return std::forward<F>(func)(std::forward<Ts>(vs)...);
        }
    };

    template <int Cur, int E, int S> struct GenIntSequence
    {
        template <typename F, typename... Ts> static constexpr auto call(F &&func, Ts &&... vs)
        {
            if constexpr (S > 0 && Cur >= E || S < 0 && Cur <= E) {
                return std::forward<F>(func)(std::forward<Ts>(vs)...);
            }
            else {
                return GenIntSequence<Cur + S, E, S>::call(std::forward<F>(func),
                                                           std::forward<Ts>(vs)...,
                                                           Cur);
            }
        }
    };

}  // namespace detail

template <typename... Ts> struct are_same : detail::are_same<Ts...>
{};

template <typename... Ts> struct head
{
    static_assert(sizeof...(Ts) > 0, "head: Empty variadic type list");
    using type = decltype(Head(std::declval<Ts>()...));
};

template <typename... Ts> struct last
{
    static_assert(sizeof...(Ts) > 0, "last: Empty variadic type list");
    using type = decltype(Last(std::declval<Ts>()...));
};

template <std::size_t Idx, typename... Ts> struct type_at
{
    static_assert(Idx < sizeof...(Ts), "type_at: Idx is out of range");
    using type = decltype(Get<Idx>(std::declval<Ts>()...));
};

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
    static_assert(sizeof...(vs) > 0, "Head: Empty variadic list");
    return detail::Head(std::forward<Ts>(vs)...);
}

template <typename... Ts> constexpr auto Last(Ts &&... vs)
{
    static_assert(sizeof...(vs) > 0, "Last: Empty variadic list");
    return detail::Last(std::forward<Ts>(vs)...);
}

template <int Idx, typename... Ts> constexpr auto Get(Ts &&... vs)
{
    constexpr int Len = sizeof...(vs);
    static_assert(-Len <= Idx && Idx < Len, "Get: Index out of range");
    return detail::NthElement<(Idx + Len) % Len, Ts...>::call(std::forward<Ts>(vs)...);
}

template <typename F, typename Acc> constexpr auto Foldl(F &&func, Acc &&init)
{
    return [func = std::forward<F>(func), init = std::forward<Acc>(init)](auto &&... vs) mutable {
        if constexpr (sizeof...(vs) == 0) {
            return std::forward<decltype(init)>(init);
        }
        else {
            return detail::Foldl(std::forward<decltype(func)>(func),
                                 std::forward<decltype(init)>(init),
                                 std::forward<decltype(vs)>(vs)...);
        }
    };
}

template <typename F> constexpr auto Foldl(F &&func)
{
    return [func = std::forward<F>(func)](auto &&... vs) mutable {
        static_assert(sizeof...(vs) >= 2,
                      "Foldl: Variadic list should contain at least 2 elements");

        return detail::Foldl(std::forward<decltype(func)>(func), std::forward<decltype(vs)>(vs)...);
    };
}

template <typename F, typename Acc> constexpr auto Foldr(F &&func, Acc &&init)
{
    return [func = std::forward<F>(func), init = std::forward<Acc>(init)](auto &&... vs) mutable {
        if constexpr (sizeof...(vs) == 0) {
            return std::forward<decltype(init)>(init);
        }
        else {
            return detail::Foldr(std::forward<decltype(func)>(func),
                                 std::forward<decltype(vs)>(vs)...,
                                 std::forward<decltype(init)>(init));
        }
    };
}

template <typename F> constexpr auto Foldr(F &&func)
{
    return [func = std::forward<F>(func)](auto &&... vs) mutable {
        static_assert(sizeof...(vs) >= 2,
                      "Foldr: Variadic list should contain at least 2 elements");

        return detail::Foldr(std::forward<decltype(func)>(func), std::forward<decltype(vs)>(vs)...);
    };
}

template <typename F> constexpr auto Exists(F &&func)
{
    return [func = std::forward<F>(func)](auto &&... vs) mutable {
        return (std::forward<decltype(func)>(func)(std::forward<decltype(vs)>(vs)) || ...);
    };
}

template <typename F> constexpr auto All(F &&func)
{
    return [func = std::forward<F>(func)](auto &&... vs) mutable {
        return (std::forward<decltype(func)>(func)(std::forward<decltype(vs)>(vs)) && ...);
    };
}

template <typename F> constexpr auto ForEach(F &&func)
{
    return [func = std::forward<F>(func)](auto &&... vs) mutable {
        (std::forward<decltype(func)>(func)(std::forward<decltype(vs)>(vs)), ...);
    };
}

template <typename F> constexpr auto FindFirst(F &&func)
{
    return [func = std::forward<F>(func)](auto &&... vs) mutable {
        static_assert(sizeof...(vs) > 0, "FindFirst: Empty variadic list");
        static_assert(are_same_v<decltype(vs)...>, "FindFirst: Variadic list must be homogeneous");

        for (auto &&v : {std::forward<decltype(vs)>(vs)...}) {
            if (std::forward<decltype(func)>(func)(std::forward<decltype(v)>(v))) {
                return std::optional {v};
            }
        }
        return std::optional<decltype(Get<0>(vs...))> {};
    };
}

template <typename F> constexpr auto Reverse(F &&funcToForward)
{
    return [func = std::forward<F>(funcToForward)](auto &&... vs) mutable {
        return detail::Reverse<sizeof...(vs)>::call(std::forward<decltype(func)>(func),
                                                    std::forward<decltype(vs)>(vs)...);
    };
}

template <std::size_t N, typename F> constexpr auto RotateLeft(F &&funcToForward)
{
    return [func = std::forward<F>(funcToForward)](auto &&... vs) mutable {
        constexpr std::size_t ShiftAmount = sizeof...(vs) == 0 ? 0 : N % sizeof...(vs);

        return detail::RotateLeft<ShiftAmount>::call(std::forward<decltype(func)>(func),
                                                     std::forward<decltype(vs)>(vs)...);
    };
}

template <std::size_t N, typename F> constexpr auto RotateRight(F &&funcToForward)
{
    return [func = std::forward<F>(funcToForward)](auto &&... vs) mutable {
        constexpr std::size_t ShiftAmount =
            sizeof...(vs) == 0 ? 0 : sizeof...(vs) - (N % sizeof...(vs));

        return detail::RotateLeft<ShiftAmount>::call(std::forward<decltype(func)>(func),
                                                     std::forward<decltype(vs)>(vs)...);
    };
}

template <int N, typename F> constexpr auto Rotate(F &&funcToForward)
{
    if constexpr (N < 0) {
        return RotateRight<-N>(std::forward<F>(funcToForward));
    }
    else {
        return RotateLeft<N>(std::forward<F>(funcToForward));
    }
}

template <std::size_t I, std::size_t J, typename F> constexpr auto Swap(F &&funcToForward)
{
    return [func = std::forward<F>(funcToForward)](auto &&... vs) mutable {
        constexpr std::size_t L = sizeof...(vs);
        static_assert(I < L, "Swap: Index I out of range");
        static_assert(J < L, "Swap: Index J out of range");

        return detail::Swap<L - I, L - J, L>::call(std::forward<decltype(func)>(func),
                                                   Get<I>(std::forward<decltype(vs)>(vs)...),
                                                   Get<J>(std::forward<decltype(vs)>(vs)...),
                                                   std::forward<decltype(vs)>(vs)...);
    };
}

template <int N, typename F> constexpr auto Take(F &&funcToForward)
{
    if constexpr (N < 0) {
        return [func = std::forward<F>(funcToForward)](auto &&... vs) mutable {
            static_assert(-N <= sizeof...(vs), "Take: -N is greater than the number of arguments");

            return detail::DropFirst<sizeof...(vs) - (-N)>::call(std::forward<decltype(func)>(func),
                                                                 std::forward<decltype(vs)>(vs)...);
        };
    }
    else {
        return RotateLeft<N>([func = std::forward<F>(funcToForward)](auto &&... vs) mutable {
            static_assert(N <= sizeof...(vs), "Take: N is greater than the number of arguments");

            return detail::DropFirst<sizeof...(vs) - N>::call(std::forward<decltype(func)>(func),
                                                              std::forward<decltype(vs)>(vs)...);
        });
    }
}

template <int N, typename F> constexpr auto Drop(F &&funcToForward)
{
    if constexpr (N < 0) {
        return RotateRight<-N>([func = std::forward<F>(funcToForward)](auto &&... vs) mutable {
            static_assert(-N <= sizeof...(vs), "Drop: -N is greater than the number of arguments");

            return detail::DropFirst<-N>::call(std::forward<decltype(func)>(func),
                                               std::forward<decltype(vs)>(vs)...);
        });
    }
    else {
        return [func = std::forward<F>(funcToForward)](auto &&... vs) mutable {
            static_assert(N <= sizeof...(vs), "Drop: N is greater than the number of arguments");

            return detail::DropFirst<N>::call(std::forward<decltype(func)>(func),
                                              std::forward<decltype(vs)>(vs)...);
        };
    }
}

template <int N, int M, typename F> constexpr auto Slice(F &&funcToForward)
{
    static_assert(N <= M, "Slice: N must be not greater than M");
    return [func = std::forward<F>(funcToForward)](auto &&... vs) mutable {
        static_assert(M - N <= sizeof...(vs),
                      "Slice: Slice size is greater than the number of arguments");
        if constexpr (N < 0)
            static_assert(-N <= sizeof...(vs), "Slice: -N is greater than the number of arguments");
        if constexpr (M >= 0)
            static_assert(M <= sizeof...(vs), "Slice: M is greater than the number of arguments");

        return Rotate<N>(Take<M - N>(std::forward<decltype(func)>(func)))(
            std::forward<decltype(vs)>(vs)...);
    };
}

template <typename Mapper, typename F> constexpr auto Map(Mapper &&mapper, F &&funcToForward)
{
    return [mapper = std::forward<Mapper>(mapper),
            func   = std::forward<F>(funcToForward)](auto &&... vs) mutable {
        return std::forward<decltype(func)>(func)(
            std::forward<decltype(mapper)>(mapper)(std::forward<decltype(vs)>(vs))...);
    };
}

template <typename Pred, typename F> constexpr auto Filter(Pred &&pred, F &&funcToForward)
{
    return [pred = std::forward<Pred>(pred),
            func = std::forward<F>(funcToForward)](auto &&... vs) mutable {
        return detail::Filter<sizeof...(vs)>::call(std::forward<decltype(func)>(func),
                                                   std::forward<decltype(pred)>(pred),
                                                   std::forward<decltype(vs)>(vs)...);
    };
}

template <int Idx, typename Val, typename F>
constexpr auto ReplaceAt(Val &&value, F &&funcToForward)
{
    return [val  = std::forward<Val>(value),
            func = std::forward<F>(funcToForward)](auto &&... vs) mutable {
        static_assert(-Idx <= sizeof...(vs) && Idx < sizeof...(vs),
                      "ReplaceAt: Idx is out of range");

        return detail::ReplaceAt < Idx < 0 ? -Idx : sizeof...(vs) - Idx,
               sizeof...(vs) > ::call(std::forward<decltype(func)>(func),
                                      std::forward<decltype(val)>(val),
                                      std::forward<decltype(vs)>(vs)...);
    };
}

template <int... Idx, typename F> constexpr auto Permute(F &&funcToForward)
{
    return [func = std::forward<F>(funcToForward)](auto &&... vs) mutable {
        constexpr int Len = sizeof...(vs);
        static_assert(((-Len <= Idx && Idx < Len) && ...), "Permute: Idx is out of range");

        return std::forward<decltype(func)>(func)(Get<Idx>(std::forward<decltype(vs)>(vs)...)...);
    };
}

template <std::size_t N, typename F> constexpr auto IndexSequence(F &&funcToForward)
{
    return detail::GenIndexSequence<N>::call(std::forward<F>(funcToForward));
}

template <int Begin, int End, int Step, typename F>
constexpr auto IntSequence(F &&funcToForward)
{
    static_assert(Step != 0, "IntSequence: Step must not be zero");
    static_assert(Step > 0 && Begin <= End || Step < 0 && Begin >= End,
                  "IntSequence: Invalid Range");

    return detail::GenIntSequence<Begin, End, Step>::call(std::forward<F>(funcToForward));
}

}  // namespace ftc::variadic
