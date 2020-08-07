/**
 * @file OverloadSet.hpp
 * Overload Set
 *
 * Provides a utility class for implementing overload set.
 */

#pragma once

namespace ftc {

/// Struct for holding an overload set
template <typename... F> struct OverloadSet : public F...
{
    OverloadSet(F &&... f) : F(std::forward<F>(f))... {}
    using F::operator()...;
};

/// Generates an overload set from a list of functions
/// @param f A list of functions to overload
template <typename... F> OverloadSet<F...> Overload(F &&... f)
{
    return OverloadSet<F...>(std::forward<F>(f)...);
}

}  // namespace ftc