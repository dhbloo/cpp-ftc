#include <cassert>
#include <memory>

template <typename> class static_function; /* undefined */

template <typename Result, typename... Args> class static_function<Result(Args...)>
{
public:
    static_function() = default;
    template <typename T> static_function(T &&t) { operator=(std::forward<T &&>(t)); }

    template <typename T> static_function &operator=(T t)
    {
        static_assert(sizeof(CallableT<T>) <= sizeof(buffer), "function is too large for buffer");
        callable.reset(new (buffer) CallableT<T>(t));
        return *this;
    }

    Result operator()(Args... args) const
    {
        assert(callable);
        return callable->Invoke(args...);
    }

private:
    class ICallable
    {
    public:
        virtual ~ICallable()                = default;
        virtual Result Invoke(Args... args) = 0;
    };

    template <typename T> class CallableT : public ICallable
    {
    public:
        CallableT(const T &t) : t(t) {}
        ~CallableT() override = default;

        Result Invoke(Args... args) override { return t(args...); }

    private:
        T t;
    };

    struct Deleter
    {
        void operator()(ICallable *callable) { callable->~ICallable(); }
    };

    char                                buffer[16];
    std::unique_ptr<ICallable, Deleter> callable;  // used 8 bytes
};
