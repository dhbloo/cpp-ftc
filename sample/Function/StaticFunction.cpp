#include "Function/StaticFunction.hpp"

#include "Debug/ObjBehaviourTester.hpp"

#include <iostream>
#include <memory>
#include <string>

using namespace cpppg;

void func()
{
    std::cout << "func" << std::endl;
}

struct functor
{
    void operator()() { std::cout << "functor" << std::endl; }
};

void sample1()
{
    std::cout << "sizeof StaticFunction<void()>: " << sizeof(StaticFunction<void()>) << std::endl;
    StaticFunction<void()> f(func);
    f();
    f = functor();
    f();
    f = [] { std::cout << "lambda" << std::endl; };
    f();

    char array[24] = "123";
    f              = [array]() mutable {
        array[0]++;
        std::cout << array << std::endl;
    };
    f();

    StaticFunction<void()> f2;
    f2 = f;
    f2();

    StaticFunction<void()> f3(f2);
    f3();

    StaticFunction<void()> f4 = f2;
    f4();

    StaticFunction<void(), 64> f5(f4);
    f5();

    StaticFunction<void(), 64> f6 = f4;
    f6();

    StaticFunction<void(), 64> f7(nullptr);
    try {
        f7();
        std::cout << "Should not print." << std::endl;
    }
    catch (...) {
        std::cout << "f7 is empty." << std::endl;
    }

    f7 = f4;
    f7();

    f7 = f7;
    f7();

    std::cout << "Before, f3 is not empty: " << (bool)f3 << std::endl;
    StaticFunction<void()> f8(std::move(f3));
    std::cout << "After, f3 is not empty: " << (bool)f3 << std::endl;
    f8();

    std::cout << "Before, f8 is not empty: " << (bool)f8 << std::endl;
    StaticFunction<void()> f9;
    f9 = std::move(f8);
    std::cout << "After, f8 is not empty: " << (bool)f8 << std::endl;
    f9();

    std::cout << "Before, f8 is not empty: " << (bool)f9 << std::endl;
    StaticFunction<void(), 64> f10(std::move(f9));
    std::cout << "After, f8 is not empty: " << (bool)f9 << std::endl;
    f10();

    std::cout << "Before, f4 is not empty: " << (bool)f4 << std::endl;
    StaticFunction<void(), 64> f11;
    f11 = std::move(f4);
    std::cout << "After, f4 is not empty: " << (bool)f4 << std::endl;
    f11();
}

void sample2()
{
    auto lambda1 = [obj = ObjBehaviourTester<>("1")]() {};
    auto lambda2 = [obj = ObjBehaviourTester<>("2")]() {};
    auto lambda3 = [obj = ObjBehaviourTester<>("3")]() {};
    auto lambda4 = [obj = ObjBehaviourTester<>("4")]() {};

    std::cout << "====================" << std::endl;

    StaticFunction<void()> f1(lambda1);
    StaticFunction<void()> f2(std::move(lambda2));
    StaticFunction<void()> f3;
    f3 = lambda3;
    StaticFunction<void()> f4;
    f4 = std::move(lambda4);

    f1();
    f2();
    f3();
    f4();

    std::cout << "====================" << std::endl;

    StaticFunction<void()> f5(f1);
    StaticFunction<void()> f6(std::move(f2));
    StaticFunction<void()> f7;
    f7 = f3;
    StaticFunction<void()> f8;
    f8 = std::move(f4);

    f1();
    f3();
    f5();
    f6();
    f7();
    f8();

    std::cout << "====================" << std::endl;
}
void sample3()
{
    {
        auto f = [str = std::string("A string")]() mutable {
            std::cout << str << ", " << (void *)str.c_str() << std::endl;
        };
        f();
        auto g = f;
        g();
    }

    StaticFunction<void(), sizeof(std::string)> g;
    {
        StaticFunction<void(), sizeof(std::string)> f;
        f = [str = std::string("A string")]() mutable {
            std::cout << str << ", " << (void *)str.c_str() << std::endl;
        };
        f();
        g = f;
    }
    g();

    /*auto f = [p = std::make_unique<int>(42)] { return *p; };
    StaticFunction<int()> x(std::move(f));
    std::cout << x() << std::endl;

    StaticFunction<int()> y;
    y = std::move(x);
    std::cout << y() << std::endl;*/
}

int main()
{
    sample1();
    std::cout << std::endl;
    sample2();
    std::cout << std::endl;
    sample3();
}