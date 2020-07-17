#pragma once

#include "Fwd.hpp"

/// Internal macro: return expr in a lambda
#define __RETURNS(expr) \
    noexcept(noexcept(expr))->decltype(auto) { return (expr); }

/// A helper to pass overloaded function as an argument.
///
/// If the overloaded function is passed directly, template deduction will fail.
/// A workaround is to have it warpped by a lambda, which is done by this macro.
///
/// @see https://florianjw.de/en/passing_overloaded_functions.html
/// @see https://akrzemi1.wordpress.com/2018/07/07/functions-in-std/
#define LIFT(...) [](auto &&... args) __RETURNS(__VA_ARGS__(FWD(args)...))

/// A helper to pass overloaded unary operator as an argument.
#define LIFT_UNARY_OP(op) [](auto &&arg1) __RETURNS(op FWD(arg1))

/// A helper to pass overloaded (post) unary operator as an argument.
///
/// Note: This can be used to access member of an object, eg. LIFT_POST_UNARY_OP(.xxx), or
///       invoke a member function, eg. LIFT_POST_UNARY_OP(.fun(xxx...))
#define LIFT_POST_UNARY_OP(op) [](auto &&arg1) __RETURNS(FWD(arg1) op)

/// A helper to pass overloaded binary operator as an argument.
#define LIFT_BINARY_OP(op) [](auto &&arg1, auto &&arg2) __RETURNS(FWD(arg1) op FWD(arg2))
