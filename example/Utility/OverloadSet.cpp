#include "FTC/Utility/OverloadSet.hpp"

#include <iostream>

int main()
{
    auto f = ftc::Overload([](int a) { std::cout << a << '\n'; },
                           [](double a) { std::cout << a << '\n'; });
    auto g = ftc::OverloadSet {[](int a) { std::cout << a << '\n'; },
                               [](double a) { std::cout << a << '\n'; }};

    f(2);
    f(5.0);
    g(3);
    g(6.0);
}