#include "Template/Variadic.hpp"

#include <iostream>

//static_assert(__cplusplus == 201703L);

using namespace cpppg::variadic;

#define EVAL(...) std::cout << #__VA_ARGS__ << " = " << (__VA_ARGS__) << '\n';

int main()
{
    EVAL(Get<-1>(1, 2, 3, 4, 5));

    int  base = 10;
    auto f    = [=](int a, int b) { return base * a + b; };
    auto m    = [](auto x) { return x + 1; };
    auto pred = [](auto x) { return x >= 2; };
    EVAL(IndexSequence<5>(Drop<1>(Filter(pred, RotateRight<0>(Map(m, Foldl(f, 0)))))));

    int a = 1, b = 2, c = 3, d = 4;
    EVAL(Map(m, FindFirst(pred))(a, b, c, d, 5, 6).value());

    auto print = [](auto x) { std::cout << x << ','; };
    EVAL(IntSequence<5, -5, -2>(ForEach(print)), 0);

    EVAL(typeid(type_at<2, int, short, char>).name());
}