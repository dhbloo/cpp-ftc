/**
 * @file SmallFunction.hpp
 * Small(static) Function
 *
 * A function container just like std::function, but using static buffer for allocation.
 */

#pragma once

#include <functional>  // for std::bad_function_call
#include <utility>     // for std::forward, std::move

namespace ftc {

template <typename, std::size_t BufferSize = 24> class SmallFunction; /* undefined */

namespace detail {

    template <typename T> struct __is_small_function
    {
        static constexpr bool value = false;
    };

    template <typename Result, typename... Args, std::size_t BufferSize>
    struct __is_small_function<SmallFunction<Result(Args...), BufferSize>>
    {
        static constexpr bool value = true;
    };

}  // namespace detail


/// Function container with static storage size
/// @tparam Result Function return type
/// @tparam Args Function argument types
/// @tparam BufferSize Storage size, default is 24.
template <typename Result, typename... Args, std::size_t BufferSize>
class SmallFunction<Result(Args...), BufferSize>
{
    static_assert(BufferSize >= 8, "buffer size should be at least 8");

public:
    SmallFunction() noexcept { new (storage) CallableT<std::nullptr_t>(nullptr); }
    ~SmallFunction() { ((ICallable *)storage)->~ICallable(); }

    SmallFunction(const SmallFunction &other) noexcept
    {
        const ICallable *_callable = reinterpret_cast<const ICallable *>(other.storage);
        _callable->Clone((void *)storage);
    }

    SmallFunction(SmallFunction &&other) noexcept
    {
        ICallable *_callable = reinterpret_cast<ICallable *>(other.storage);
        _callable->Move((void *)storage);
        other = nullptr;
    }

    template <std::size_t BufferSizeT>
    SmallFunction(const SmallFunction<Result(Args...), BufferSizeT> &t) noexcept
    {
        static_assert(BufferSizeT <= BufferSize, "buffer size is smaller than needed");
        const ICallable *_callable =
            reinterpret_cast<const ICallable *>(reinterpret_cast<const SmallFunction &>(t).storage);
        _callable->Clone((void *)storage);
    }

    template <std::size_t BufferSizeT>
    SmallFunction(SmallFunction<Result(Args...), BufferSizeT> &&t) noexcept
    {
        static_assert(BufferSizeT <= BufferSize, "buffer size is smaller than needed");
        SmallFunction &other     = reinterpret_cast<SmallFunction &>(t);
        ICallable *    _callable = reinterpret_cast<ICallable *>(other.storage);
        _callable->Move((void *)storage);
        other = nullptr;
    }

    template <typename T,
              typename = typename std::enable_if_t<
                  !std::is_function<T>::value
                  && !detail::__is_small_function<typename std::decay<T>::type>::value>>
    SmallFunction(T &&t) noexcept
    {
        using CallableType = typename std::decay<T>::type;
        static_assert(sizeof(CallableT<CallableType>) <= sizeof(storage),
                      "function is too large for buffer");
        new (storage) CallableT<CallableType>(std::forward<T>(t));
    }

    template <typename T,
              typename = typename std::enable_if_t<
                  !std::is_function<T>::value
                  && !detail::__is_small_function<typename std::decay<T>::type>::value>>
    SmallFunction &operator=(T &&t) noexcept
    {
        using CallableType = typename std::decay<T>::type;
        static_assert(sizeof(CallableT<CallableType>) <= sizeof(storage),
                      "function is too large for buffer");
        ((ICallable *)storage)->~ICallable();
        new (storage) CallableT<CallableType>(std::forward<T>(t));
        return *this;
    }

    SmallFunction &operator=(const SmallFunction &other) noexcept
    {
        if (this == &other)
            return *this;
        ((ICallable *)storage)->~ICallable();
        const ICallable *_callable = reinterpret_cast<const ICallable *>(other.storage);
        _callable->Clone((void *)storage);
        return *this;
    }

    SmallFunction &operator=(SmallFunction &&other) noexcept
    {
        if (this == &other)
            return *this;
        ((ICallable *)storage)->~ICallable();
        ICallable *_callable = reinterpret_cast<ICallable *>(other.storage);
        _callable->Move((void *)storage);
        other = nullptr;
        return *this;
    }

    template <std::size_t BufferSizeT>
    SmallFunction &operator=(const SmallFunction<Result(Args...), BufferSizeT> &t) noexcept
    {
        static_assert(BufferSizeT <= BufferSize, "buffer size is smaller than needed");
        const SmallFunction &other = reinterpret_cast<const SmallFunction &>(t);
        ((ICallable *)storage)->~ICallable();
        const ICallable *_callable = reinterpret_cast<const ICallable *>(other.storage);
        _callable->Clone((void *)storage);
        return *this;
    }

    template <std::size_t BufferSizeT>
    SmallFunction &operator=(SmallFunction<Result(Args...), BufferSizeT> &&t) noexcept
    {
        static_assert(BufferSizeT <= BufferSize, "buffer size is smaller than needed");
        SmallFunction &other = reinterpret_cast<SmallFunction &>(t);
        ((ICallable *)storage)->~ICallable();
        ICallable *_callable = reinterpret_cast<ICallable *>(other.storage);
        _callable->Move((void *)storage);
        other = nullptr;
        return *this;
    }

    /// Checks if a function is contained
    explicit operator bool() const noexcept { return ((ICallable *)storage)->IsNotEmpty(); }

    /// Invokes the function
    /// 
    /// Throws std::bad_function_call if no function is contained.
    /// 
    /// @param args Function arguments
    /// @return Function call result
    Result   operator()(Args... args) const { return ((ICallable *)storage)->Invoke(args...); }

    /// @name Compares a SmallFunction with nullptr
    /// @{
    friend bool operator==(const SmallFunction &f, std::nullptr_t p) noexcept { return !(bool)f; }
    friend bool operator==(std::nullptr_t p, const SmallFunction &f) noexcept { return !(bool)f; }
    friend bool operator!=(const SmallFunction &f, std::nullptr_t p) noexcept { return (bool)f; }
    friend bool operator!=(std::nullptr_t p, const SmallFunction &f) noexcept { return (bool)f; }
    /// @}

private:
    class ICallable
    {
    public:
        virtual ~ICallable() noexcept                         = default;
        virtual bool   IsNotEmpty() const noexcept            = 0;
        virtual Result Invoke(Args... args)                   = 0;
        virtual void   Clone(void *dstStorage) const noexcept = 0;
        virtual void   Move(void *dstStorage) noexcept        = 0;
    };

    template <typename T, typename Dummy = void> class CallableT : public ICallable
    {
    public:
        CallableT(const T &callable) : f(callable) {}
        CallableT(T &&callable) : f(std::move(callable)) {}
        ~CallableT() noexcept override = default;

        bool   IsNotEmpty() const noexcept override { return true; }
        Result Invoke(Args... args) override { return f(args...); }
        void   Clone(void *dst) const noexcept override { new (dst) CallableT(f); }
        void   Move(void *dst) noexcept override { new (dst) CallableT(std::move(f)); }

    private:
        T f;
    };

    template <typename Dummy> class CallableT<std::nullptr_t, Dummy> : public ICallable
    {
    public:
        CallableT(const std::nullptr_t &t) {}
        ~CallableT() noexcept override = default;

        bool   IsNotEmpty() const noexcept override { return false; }
        Result Invoke(Args... args) override { throw std::bad_function_call(); }
        void   Clone(void *dst) const noexcept override { new (dst) CallableT(nullptr); }
        void   Move(void *dst) noexcept override { new (dst) CallableT(nullptr); }
    };

    char storage[BufferSize + 8];
};

}  // namespace ftc
