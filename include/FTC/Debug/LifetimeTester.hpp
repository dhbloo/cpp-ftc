/**
 * @file LifetimeTester.hpp
 * Object Lifetime Tester
 */

#pragma once

#include <cstring>
#include <iostream>

namespace ftc {

namespace {
    struct StdoutWrapper
    {
        template <typename T> inline StdoutWrapper &operator<<(T &&t) noexcept
        {
            std::cout << t;
            return *this;
        }
    };
}  // namespace

template <typename OutputWarpper = StdoutWrapper, std::size_t NameSize = 16>
class LifetimeTester
{
public:
    LifetimeTester(const char *_name = "Unnamed") noexcept
    {
        strncpy(name, _name, sizeof(name));
        OutputWarpper() << name << ": Default constructor called.\n";
    }

    LifetimeTester(const LifetimeTester &t) noexcept
    {
        strncpy(name, t.name, sizeof(name));
        OutputWarpper() << name << ": Copy constructor called.\n";
    }
    LifetimeTester(LifetimeTester &&t) noexcept
    {
        strncpy(name, t.name, sizeof(name));
        OutputWarpper() << name << ": Move constructor called.\n";
    }

    LifetimeTester &operator=(const LifetimeTester &t) noexcept
    {
        strncpy(name, t.name, sizeof(name));
        OutputWarpper() << name << ": Copy Assignment called.\n";
    }
    LifetimeTester &operator=(LifetimeTester &&t) noexcept
    {
        strncpy(name, t.name, sizeof(name));
        OutputWarpper() << name << ": Move Assignment called.\n";
    }

    ~LifetimeTester() { OutputWarpper() << name << ": Destructor called.\n"; }

private:
    char name[NameSize];
};

}  // namespace ftc
