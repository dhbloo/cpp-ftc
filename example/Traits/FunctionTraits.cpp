#include "FTC/Traits/FunctionTraits.hpp"

#include <functional>
#include <type_traits>

using namespace ftc;

double f(int x, float y, double z)
{
    return x + y + z;
}

float g(int x, float y) noexcept
{
    return x + y;
}

struct A
{
    int    f(char x) { return x; }
    double g(float x) const { return x; }
    void   h() noexcept {}
    float  i() const noexcept { return 1.0f; }
    bool   operator()(int x) { return x > 0; }
};

struct B
{
    int   f(int x) { return x; }
    float f(float x) { return x; }
    bool  f(bool x) { return x; }
    char  f(char x, char y) { return x + y; }
};

std::function<double(int, float, double)> fo      = f;
auto                                      lambda  = [](int x) { return (float)x; };
auto                                      glambda = [](auto x) { return x; };

int main()
{
    // is callable
    static_assert(is_callable_v<void()>);
    static_assert(is_callable_v<int(int, char)>);
    static_assert(is_callable_v<decltype(f)>);
    static_assert(is_callable_v<decltype(&f)>);
    static_assert(is_callable_v<decltype(g)>);
    static_assert(is_callable_v<decltype(&g)>);
    static_assert(!is_callable_v<decltype(&A::f)>);
    static_assert(is_callable_v<A>);
    static_assert(!is_callable_v<B>);
    static_assert(is_callable_v<decltype(lambda)>);
    //static_assert(!is_callable_v<decltype(glambda)>); // VS2019 bug
    static_assert(is_callable_v<decltype(fo)>);
    static_assert(is_callable_v<decltype(std::move(fo))>);

    // native function type
    static_assert(std::is_same_v<result_of_t<int(int, char)>, int>);
    static_assert(arity_of_v<int(int, char)> == 2);
    static_assert(std::is_same_v<arg_at_t<int(int, char), 0>, int>);
    static_assert(std::is_same_v<arg_at_t<int(int, char), 1>, char>);

    // native function
    static_assert(std::is_same_v<result_of_t<decltype(f)>, double>);
    static_assert(arity_of_v<decltype(f)> == 3);
    static_assert(ArityOf(f) == 3);
    static_assert(std::is_same_v<arg_at_t<decltype(f), 0>, int>);
    static_assert(std::is_same_v<arg_at_t<decltype(f), 1>, float>);
    static_assert(std::is_same_v<arg_at_t<decltype(f), 2>, double>);

    // native function pointer
    static_assert(std::is_same_v<result_of_t<decltype(&f)>, double>);
    static_assert(arity_of_v<decltype(&f)> == 3);
    static_assert(std::is_same_v<arg_at_t<decltype(&f), 0>, int>);
    static_assert(std::is_same_v<arg_at_t<decltype(&f), 1>, float>);
    static_assert(std::is_same_v<arg_at_t<decltype(&f), 2>, double>);

    // noexcept native function
    static_assert(std::is_same_v<result_of_t<decltype(g)>, float>);
    static_assert(arity_of_v<decltype(g)> == 2);
    static_assert(ArityOf(g) == 2);
    static_assert(std::is_same_v<arg_at_t<decltype(g), 0>, int>);
    static_assert(std::is_same_v<arg_at_t<decltype(g), 1>, float>);

    // noexcept native function pointer
    static_assert(std::is_same_v<result_of_t<decltype(&g)>, float>);
    static_assert(arity_of_v<decltype(&g)> == 2);
    static_assert(ArityOf(&g) == 2);
    static_assert(std::is_same_v<arg_at_t<decltype(&g), 0>, int>);
    static_assert(std::is_same_v<arg_at_t<decltype(&g), 1>, float>);

    // member function
    static_assert(std::is_same_v<result_of_t<decltype(&A::f)>, int>);
    static_assert(arity_of_v<decltype(&A::f)> == 2);
    static_assert(std::is_same_v<arg_at_t<decltype(&A::f), 1>, char>);
    static_assert(std::is_same_v<class_of_member_func_t<decltype(&A::f)>, A>);

    // const member function
    static_assert(std::is_same_v<result_of_t<decltype(&A::g)>, double>);
    static_assert(arity_of_v<decltype(&A::g)> == 2);
    static_assert(std::is_same_v<arg_at_t<decltype(&A::g), 1>, float>);
    static_assert(std::is_same_v<class_of_member_func_t<decltype(&A::g)>, A>);

    // noexcept member function
    static_assert(std::is_same_v<result_of_t<decltype(&A::h)>, void>);
    static_assert(arity_of_v<decltype(&A::h)> == 1);
    static_assert(std::is_same_v<class_of_member_func_t<decltype(&A::h)>, A>);

    // const noexcept member function
    static_assert(std::is_same_v<result_of_t<decltype(&A::i)>, float>);
    static_assert(arity_of_v<decltype(&A::i)> == 1);
    static_assert(std::is_same_v<class_of_member_func_t<decltype(&A::i)>, A>);

    // function object
    static_assert(std::is_same_v<result_of_t<A>, bool>);
    static_assert(arity_of_v<A> == 1);
    static_assert(std::is_same_v<arg_at_t<A, 0>, int>);

    // lambda function
    static_assert(std::is_same_v<result_of_t<decltype(lambda)>, float>);
    static_assert(arity_of_v<decltype(lambda)> == 1);
    static_assert(std::is_same_v<arg_at_t<decltype(lambda), 0>, int>);

    // std::function
    static_assert(std::is_same_v<result_of_t<decltype(fo)>, double>);
    static_assert(arity_of_v<decltype(fo)> == 3);
    static_assert(std::is_same_v<arg_at_t<decltype(fo), 0>, int>);
    static_assert(std::is_same_v<arg_at_t<decltype(fo), 1>, float>);
    static_assert(std::is_same_v<arg_at_t<decltype(fo), 2>, double>);

    // std::function r-value
    static_assert(std::is_same_v<result_of_t<decltype(std::move(fo))>, double>);
    static_assert(arity_of_v<decltype(std::move(fo))> == 3);
    static_assert(std::is_same_v<arg_at_t<decltype(std::move(fo)), 0>, int>);
    static_assert(std::is_same_v<arg_at_t<decltype(std::move(fo)), 1>, float>);
    static_assert(std::is_same_v<arg_at_t<decltype(std::move(fo)), 2>, double>);
}