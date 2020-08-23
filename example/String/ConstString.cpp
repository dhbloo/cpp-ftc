#include "FTC/String/ConstString.hpp"

#include <iostream>

using namespace ftc;

template <int value, bool comma = false> constexpr auto ToLiteral()
{
    ConstString<comma> c(',');
    if constexpr (value > 10)
        return ToLiteral<value / 10, true>() + ('0' + value % 10) + c;
    else
        return ConstString<1>('0' + value) + c;
}

int main()
{
    constexpr StringLiteral s {"12345"}, s2 = s;
    std::cout << s << '\n';
    std::cout << s.Length() << '\n';
    std::cout << s.Front() << '\n';
    std::cout << s.Back() << '\n';
    std::cout << s.At<2>() << '\n';
    std::cout << s[2] << '\n';
    std::cout << (s == s2) << '\n';

    for (auto c : s2) {
        std::cout << c << ',';
    }

    constexpr ConstString cs {s};
    std::cout << cs << '\n';

    constexpr ConstString cs2 = "6789";
    constexpr ConstString cs3 = cs + cs2;
    constexpr ConstString cs4 = cs2 + cs;

    std::cout << cs3 << '\n';
    std::cout << cs4 << '\n';
    std::cout << (cs + s2) << '\n';
    std::cout << (s + cs2) << '\n';
    std::cout << (s + "abcd") << '\n';
    std::cout << ("abcd" + s2) << '\n';
    std::cout << (cs + "abcd") << '\n';
    std::cout << ("abcd" + cs2) << '\n';
    std::cout << ('a' + s + 'b') << '\n';

    for (char c : cs) {
        std::cout << c << ',';
    }

    std::printf("%s\n", ("abcd" + cs + "edf").c_str());
    std::cout << ToLiteral<123456789>() << '\n';
}
