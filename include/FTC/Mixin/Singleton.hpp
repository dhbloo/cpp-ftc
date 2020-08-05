// Singleton Mixin
//
// Integrate the singleton interface into a class using mixin. Inspired from Loki library.

#pragma once

#include "FTC/Traits/FunctionTraits.hpp"

#include <atomic>
#include <memory>
#include <mutex>

namespace ftc {

/* -------------------------------------------------------------------------
   Singleton Interface type traits & type alias
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
/// @tparam CreateFunc A function that returns a created prvalue of T
template <class T, typename Creator = void> class GlobalSingleton : public ISingleton<T>
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
template <class T, typename Creator = void> class StaticSingleton : public ISingleton<T>
{
    static_assert(std::is_same_v<result_of_t<Creator>, T>,
                  "Creator must be a function object which returns a value of type T");

public:
    /// Gets the singleton instance of class type T
    /// @return The singleton instance of class type T
    /// @note Instance is initialized on the first call of Get()
    template <typename... Ts> static T &Get()
    {
        static T instance {Creator {}()};
        return instance;
    }
};

/// Singleton Mixin using C++11 static function variable and delayed argument initialization
/// @tparam T Singleton class
template <class T, typename Creator = void> class DelayedStaticSingleton : public ISingleton<T>
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
template <class T, typename Creator = void, typename Allocator = std::allocator<T>>
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

template <class T, typename Creator> struct GlobalSingleton<T, Creator>::GlobalLifeTimePtr
{
    GlobalLifeTimePtr() noexcept { ptr = new T {Creator {}()}; }
    ~GlobalLifeTimePtr() { delete ptr; }
    operator T *() const { return ptr; }

    T *ptr;
};

template <class T> class GlobalSingleton<T, void> : public ISingleton<T>
{
public:
    static T &Get() { return *instancePtr; }

private:
    struct GlobalLifeTimePtr
    {
        GlobalLifeTimePtr() noexcept { ptr = new T; }
        ~GlobalLifeTimePtr() { delete ptr; }
        operator T *() const { return ptr; }

        T *ptr;
    };

    // inline static std::unique_ptr<T> instancePtr {std::make_unique<T>()};
    inline static GlobalLifeTimePtr instancePtr;
};

template <class T> class StaticSingleton<T, void> : public ISingleton<T>
{
public:
    template <typename... Ts> static T &Get()
    {
        static T instance;
        return instance;
    }
};

template <class T, typename Creator, typename Allocator>
struct DynamicSingleton<T, Creator, Allocator>::ExitGuard 
{
    ~ExitGuard() { DynamicSingleton::FreeInstance(); }
};

template <class T, typename Creator, typename Allocator>
template <typename AlTy, typename AlTraits, typename InCreator> 
struct DynamicSingleton<T, Creator, Allocator>::ConstructPolicy {
    static void Construct(AlTy& allocator, T* instancePtr) {
        AlTraits::construct(allocator, instancePtr, Creator {}());
    }
};

template <class T, typename Creator, typename Allocator>
template <typename AlTy, typename AlTraits> 
struct DynamicSingleton<T, Creator, Allocator>::ConstructPolicy<AlTy, AlTraits, void> {
    static void Construct(AlTy& allocator, T* instancePtr) {
        AlTraits::construct(allocator, instancePtr);
    }
};

template <class T, typename Creator, typename Allocator>
T &DynamicSingleton<T, Creator, Allocator>::Get()
{
    T* ptr = instancePtr.load(std::memory_order_acquire);
    if (!ptr) {
        std::scoped_lock lock(mutex);
        ptr = instancePtr.load(std::memory_order_relaxed);
        if (!ptr) {
            ptr = AllocTraits::allocate(allocator, sizeof(T));
            //AllocTraits::construct(allocator, newInstance, Creator {}());
            ConstructPolicy<AllocTy, AllocTraits, Creator>::Construct(allocator, ptr);
            instancePtr.store(ptr, std::memory_order_release);
        }
    }
    return *ptr;
}

template <class T, typename Creator, typename Allocator>
void DynamicSingleton<T, Creator, Allocator>::FreeInstance()
{
    T* ptr = instancePtr.load(std::memory_order_acquire);
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