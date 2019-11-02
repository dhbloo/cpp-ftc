#include "static_function.hpp"

#include <iostream>

void func()
{
    std::cout << "func" << std::endl;
}

struct functor
{
    void operator()() { std::cout << "functor" << std::endl; }
};

int main()
{
    std::cout << "sizeof static_function<void()>: " << sizeof(static_function<void()>) << std::endl;
    static_function<void()> f(func);
    f();
    f = functor();
    f();
    f = [] { std::cout << "lambda" << std::endl; };
    f();

    char array[8] = "123";
    f             = [array] { std::cout << array << std::endl; };
    f();
}