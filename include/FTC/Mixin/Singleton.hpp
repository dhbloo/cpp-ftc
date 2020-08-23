/**
 * @file Singleton.hpp
 * Singleton Mixins
 *
 * Integrate the singleton interface into a class using mixin. Inspired from Loki library.
 */

#pragma once

#include "FTC/Traits/FunctionTraits.hpp"

#include <atomic>  // for std::atomic
#include <memory>  // for std::allocator_traits
#include <mutex>   // for std::mutex

namespace ftc {

template <typename T> struct DefaultCreator
{
    constexpr T operator()() const { return T {}; }
};

/* -------------------------------------------------------------------------
   Singleton Interface & Mixins
   ------------------------------------------------------------------------- */

/// Singleton Interface
/// @tparam T Singleton class
template <class T> class ISingleton
{
public:
    /// Gets the singleton instance of class type T
    /// @return The singleton instance of class type T
    static T &Get();
};

/// Singleton Mixin with global lifetime
/// @tparam T Singleton class
/// @tparam CreateFunc A functor that returns a created prvalue of T
template <class T, typename Creator = DefaultCreator<T>> class GlobalSingleton : public ISingleton<T>
{
    static_assert(std::is_same_v<result_of_t<Creator>, T>,
                  "Creator must be a function object which returns a value of type T");

public:
    static T &Get() { return holder.GetInstance(); }

private:
    struct InstanceHolder;
    inline static InstanceHolder holder;
};

/// Singleton Mixin using C++11 static function variable
/// @tparam T Singleton class
/// @tparam CreateFunc A functor that returns a created prvalue of T
template <class T, typename Creator = DefaultCreator<T>> class StaticSingleton : public ISingleton<T>
{
    static_assert(std::is_same_v<result_of_t<Creator>, T>,
                  "Creator must be a function object which returns a value of type T");

public:
    /// Gets the singleton instance of class type T
    /// @return The singleton instance of class type T
    /// @note Instance is initialized on the first call of Get()
    static T &Get()
    {
        static T instance {Creator {}()};
        return instance;
    }
};

/// Singleton Mixin using C++11 static function variable and delayed argument initialization
/// @tparam T Singleton class
template <class T> class DelayedStaticSingleton : public ISingleton<T>
{
public:
    /// Gets the singleton instance of class type T, the initialization is guaranteed to be
    /// thread-safe.
    /// @param args Constructor arguments
    /// @note Constructor only takes argument(s) of the first call.
    template <typename... Ts> static T &Get(Ts... args...)
    {
        static T instance {std::forward<Ts>(args)...};
        return instance;
    }
};

/// Singleton Mixin with explicit lifetime control (thread-safe)
/// @tparam T Singleton class
/// @tparam CreateFunc A functor that returns a created prvalue of T
/// @tparam Allocator Allocator to use for dynamic allocation
template <class T, typename Creator = DefaultCreator<T>, typename Allocator = std::allocator<T>>
class DynamicSingleton : public ISingleton<T>
{
public:
    /// Gets the singleton instance of class type T
    /// @return The singleton instance of class type T
    /// @note An instance will be created if one does not exist when called
    static T &Get();

    /// Destroy the created instance and free its space if there is one
    static void FreeInstance();

private:
    using AllocTy     = typename std::allocator_traits<Allocator>::template rebind_alloc<T>;
    using AllocTraits = typename std::allocator_traits<Allocator>::template rebind_traits<T>;

    struct ExitGuard;
    template <typename AlTy, typename AlTraits, typename InCreator> struct ConstructPolicy;

    inline static AllocTy          allocator;
    inline static std::atomic<T *> instancePtr;
    inline static std::mutex       mutex;
    inline static ExitGuard        exitGuard;
};

}  // namespace ftc

///////////////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////////////

namespace ftc {

template <class T, typename Creator> struct GlobalSingleton<T, Creator>::InstanceHolder
{
    InstanceHolder()
    {
        static T instance {Creator {}()};
        ptr = &instance;
    }
    T &GetInstance() { return *ptr; }
    T *ptr;
};

template <class T, typename Creator, typename Allocator>
struct DynamicSingleton<T, Creator, Allocator>::ExitGuard
{
    ~ExitGuard() { DynamicSingleton::FreeInstance(); }
};

template <class T, typename Creator, typename Allocator>
template <typename AlTy, typename AlTraits, typename InCreator>
struct DynamicSingleton<T, Creator, Allocator>::ConstructPolicy
{
    static void Construct(AlTy &allocator, T *instancePtr)
    {
        ::new (static_cast<void *>(instancePtr)) T(InCreator {}());
        //AlTraits::construct(allocator, instancePtr, InCreator {}());
    }
};

template <class T, typename Creator, typename Allocator>
template <typename AlTy, typename AlTraits>
struct DynamicSingleton<T, Creator, Allocator>::ConstructPolicy<AlTy, AlTraits, DefaultCreator<T>>
{
    static void Construct(AlTy &allocator, T *instancePtr)
    {
        AlTraits::construct(allocator, instancePtr);
    }
};

template <class T, typename Creator, typename Allocator>
T &DynamicSingleton<T, Creator, Allocator>::Get()
{
    T *ptr = instancePtr.load(std::memory_order_acquire);
    if (!ptr) {
        std::scoped_lock lock(mutex);
        ptr = instancePtr.load(std::memory_order_relaxed);
        if (!ptr) {
            ptr = AllocTraits::allocate(allocator, sizeof(T));
            ConstructPolicy<AllocTy, AllocTraits, Creator>::Construct(allocator, ptr);
            instancePtr.store(ptr, std::memory_order_release);
        }
    }
    return *ptr;
}

template <class T, typename Creator, typename Allocator>
void DynamicSingleton<T, Creator, Allocator>::FreeInstance()
{
    T *ptr = instancePtr.load(std::memory_order_acquire);
    if (ptr) {
        std::scoped_lock lock(mutex);
        ptr = instancePtr.load(std::memory_order_relaxed);
        if (ptr) {
            AllocTraits::destroy(allocator, ptr);
            AllocTraits::deallocate(allocator, ptr, sizeof(T));
            instancePtr.store(nullptr, std::memory_order_release);
        }
    }
}

}  // namespace ftc