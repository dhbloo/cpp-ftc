/**
 * @file Lift.hpp
 * Lift Macros
 * 
 * A set of macros that lift an overloaded set to a regular object which can be passed as an
 * argument.
 */

#pragma once

/// A helper to forward variadic arguments.
#define __FWD(...) std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__)

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
#define LIFT(...) [](auto &&... args) __RETURNS(__VA_ARGS__(__FWD(args)...))

/// A helper to pass overloaded unary operator as an argument.
#define LIFT_UNARY_OP(op) [](auto &&arg1) __RETURNS(op __FWD(arg1))

/// A helper to pass overloaded (post) unary operator as an argument.
///
/// @note This can be used to access member of an object, eg. LIFT_POST_UNARY_OP(.xxx), or
///       invoke a member function, eg. LIFT_POST_UNARY_OP(.fun(xxx...))
#define LIFT_POST_UNARY_OP(op) [](auto &&arg1) __RETURNS(__FWD(arg1) op)

/// A helper to pass overloaded binary operator as an argument.
#define LIFT_BINARY_OP(op) [](auto &&arg1, auto &&arg2) __RETURNS(__FWD(arg1) op __FWD(arg2))
