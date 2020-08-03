// Singleton Mixin
//
// Integrate the singleton interface into a class using mixin. Inspired from Loki library.

#pragma once

#include "FTC/Traits/FunctionTraits.hpp"

namespace ftc {

/* -------------------------------------------------------------------------
   Singleton Interface type traits & type alias
   ------------------------------------------------------------------------- */

/// Singleton Interface
/// @tparam T Singleton class
template <class T> class ISingleton
{
public:
    /// Returns a singleton instance of class type T
    static T &Get();
};

/// Singleton Mixin with global lifetime
/// @tparam T Singleton class
/// @tparam CreateFunc A function that returns a created prvalue of T
template <class T, typename Creator = nullptr_t> class GlobalSingleton : public ISingleton<T>
{
    static_assert(std::is_same_v<result_of_t<Creator>, T>,
                  "Creator must be a function object which returns a value of type T");

public:
    static T &Get() { return *instancePtr; }

private:
    struct GlobalLifeTimePtr;

    // inline static std::unique_ptr<T> instancePtr {std::make_unique<T>(std::move(CreateFunc()))};
    inline static GlobalLifeTimePtr instancePtr;
};

/// Singleton Mixin using C++11 static function variable
/// @tparam T Singleton class
template <class T, typename Creator = nullptr_t> class StaticSingleton : public ISingleton<T>
{
    static_assert(std::is_same_v<result_of_t<Creator>, T>,
                  "Creator must be a function object which returns a value of type T");

public:
    template <typename... Ts> static T &Get()
    {
        static T instance {std::move(Creator())};
        return instance;
    }
};

/// Singleton Mixin using C++11 static function variable and delayed argument initialization
/// @tparam T Singleton class
template <class T> class DelayedStaticSingleton : public ISingleton<T>
{
public:
    /// Returns a singleton instance of class type T, the initialization is guaranteed to be
    /// thread-safe.
    /// @param args Constructor arguments
    /// @note Constructor only takes argument(s) of the first call.
    template <typename... Ts> static T &Get(Ts... args...)
    {
        static T instance {std::forward<Ts>(args)...};
        return instance;
    }
};

}  // namespace ftc

///////////////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////////////

namespace ftc {

template <class T, typename Creator> struct GlobalSingleton<T, Creator>::GlobalLifeTimePtr
{
    GlobalLifeTimePtr() { ptr = new T(std::move(Creator {}())); }
    ~GlobalLifeTimePtr() { delete ptr; }
    operator T *() const { return ptr; }

    T *ptr;
};

template <class T> class GlobalSingleton<T, nullptr_t> : public ISingleton<T>
{
public:
    static T &Get() { return *instancePtr; }

private:
    struct GlobalLifeTimePtr
    {
        GlobalLifeTimePtr() { ptr = new T; }
        ~GlobalLifeTimePtr() { delete ptr; }
        operator T *() const { return ptr; }

        T *ptr;
    };

    // inline static std::unique_ptr<T> instancePtr {std::make_unique<T>()};
    inline static GlobalLifeTimePtr instancePtr;
};

template <class T> class StaticSingleton<T, nullptr_t> : public ISingleton<T>
{
public:
    template <typename... Ts> static T &Get()
    {
        static T instance;
        return instance;
    }
};

}  // namespace ftc