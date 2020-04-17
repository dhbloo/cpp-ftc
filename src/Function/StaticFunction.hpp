#pragma once
#include <functional>  // for std::bad_function_call
#include <utility>     // for std::forward, std::move

namespace cpppg {

template <typename, std::size_t BufferSize = 24> class StaticFunction; /* undefined */

namespace {

    template <typename T> struct __is_static_function
    {
        static constexpr bool value = false;
    };

    template <typename Result, typename... Args, std::size_t BufferSize>
    struct __is_static_function<StaticFunction<Result(Args...), BufferSize>>
    {
        static constexpr bool value = true;
    };

}

template <typename Result, typename... Args, std::size_t BufferSize>
class StaticFunction<Result(Args...), BufferSize>
{
    static_assert(BufferSize >= 8, "buffer size should be at least 8");

public:
    StaticFunction() noexcept { new (storage) CallableT<std::nullptr_t>(nullptr); }
    ~StaticFunction() { ((ICallable *)storage)->~ICallable(); }

    StaticFunction(const StaticFunction &other) noexcept
    {
        const ICallable *_callable = reinterpret_cast<const ICallable *>(other.storage);
        _callable->Clone((void *)storage);
    }

    StaticFunction(StaticFunction &&other) noexcept
    {
        ICallable *_callable = reinterpret_cast<ICallable *>(other.storage);
        _callable->Move((void *)storage);
        other = nullptr;
    }

    template <std::size_t BufferSizeT>
    StaticFunction(const StaticFunction<Result(Args...), BufferSizeT> &t) noexcept
    {
        static_assert(BufferSizeT <= BufferSize, "buffer size is smaller than needed");
        const ICallable *_callable = reinterpret_cast<const ICallable *>(
            reinterpret_cast<const StaticFunction &>(t).storage);
        _callable->Clone((void *)storage);
    }

    template <std::size_t BufferSizeT>
    StaticFunction(StaticFunction<Result(Args...), BufferSizeT> &&t) noexcept
    {
        static_assert(BufferSizeT <= BufferSize, "buffer size is smaller than needed");
        StaticFunction &other     = reinterpret_cast<StaticFunction &>(t);
        ICallable *      _callable = reinterpret_cast<ICallable *>(other.storage);
        _callable->Move((void *)storage);
        other = nullptr;
    }

    template <typename T,
              typename = typename std::enable_if_t<
                  !std::is_function<T>::value
                  && !__is_static_function<typename std::decay<T>::type>::value>>
    StaticFunction(T &&t) noexcept
    {
        using CallableType = typename std::decay<T>::type;
        static_assert(sizeof(CallableT<CallableType>) <= sizeof(storage),
                      "function is too large for buffer");
        new (storage) CallableT<CallableType>(std::forward<T>(t));
    }

    template <typename T,
              typename = typename std::enable_if_t<
                  !std::is_function<T>::value
                  && !__is_static_function<typename std::decay<T>::type>::value>>
    StaticFunction &operator=(T &&t) noexcept
    {
        using CallableType = typename std::decay<T>::type;
        static_assert(sizeof(CallableT<CallableType>) <= sizeof(storage),
                      "function is too large for buffer");
        ((ICallable *)storage)->~ICallable();
        new (storage) CallableT<CallableType>(std::forward<T>(t));
        return *this;
    }

    StaticFunction &operator=(const StaticFunction &other) noexcept
    {
        if (this == &other)
            return *this;
        ((ICallable *)storage)->~ICallable();
        const ICallable *_callable = reinterpret_cast<const ICallable *>(other.storage);
        _callable->Clone((void *)storage);
        return *this;
    }

    StaticFunction &operator=(StaticFunction &&other) noexcept
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
    StaticFunction &operator=(const StaticFunction<Result(Args...), BufferSizeT> &t) noexcept
    {
        static_assert(BufferSizeT <= BufferSize, "buffer size is smaller than needed");
        const StaticFunction &other = reinterpret_cast<const StaticFunction &>(t);
        ((ICallable *)storage)->~ICallable();
        const ICallable *_callable = reinterpret_cast<const ICallable *>(other.storage);
        _callable->Clone((void *)storage);
        return *this;
    }

    template <std::size_t BufferSizeT>
    StaticFunction &operator=(StaticFunction<Result(Args...), BufferSizeT> &&t) noexcept
    {
        static_assert(BufferSizeT <= BufferSize, "buffer size is smaller than needed");
        StaticFunction &other = reinterpret_cast<StaticFunction &>(t);
        ((ICallable *)storage)->~ICallable();
        ICallable *_callable = reinterpret_cast<ICallable *>(other.storage);
        _callable->Move((void *)storage);
        other = nullptr;
        return *this;
    }

    explicit operator bool() const noexcept { return ((ICallable *)storage)->IsNotEmpty(); }
    Result   operator()(Args... args) const { return ((ICallable *)storage)->Invoke(args...); }

    friend bool operator==(const StaticFunction &f, std::nullptr_t p) noexcept { return !(bool)f; }
    friend bool operator==(std::nullptr_t p, const StaticFunction &f) noexcept { return !(bool)f; }
    friend bool operator!=(const StaticFunction &f, std::nullptr_t p) noexcept { return (bool)f; }
    friend bool operator!=(std::nullptr_t p, const StaticFunction &f) noexcept { return (bool)f; }

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

}  // namespace cpppg