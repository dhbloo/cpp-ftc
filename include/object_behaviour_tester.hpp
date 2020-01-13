#pragma once
#include <cstring>
#include <iostream>

struct stdout_warpper
{
    template <typename T> inline stdout_warpper &operator<<(T &&t) noexcept
    {
        std::cout << t;
        return *this;
    }
};

template <typename output_warpper = stdout_warpper, std::size_t NameSize = 8>
class object_behaviour_tester
{
public:
    object_behaviour_tester(const char *_name = "Unnamed")
    {
        strncpy(name, _name, sizeof(name));
        output_warpper() << name << ": Default constructor called.\n";
    }

    object_behaviour_tester(const object_behaviour_tester &t)
    {
        strncpy(name, t.name, sizeof(name));
        output_warpper() << name << ": Copy constructor called.\n";
    }
    object_behaviour_tester(object_behaviour_tester &&t)
    {
        strncpy(name, t.name, sizeof(name));
        output_warpper() << name << ": Move constructor called.\n";
    }

    object_behaviour_tester &operator=(const object_behaviour_tester &t)
    {
        strncpy(name, t.name, sizeof(name));
        output_warpper() << name << ": Copy Assignment called.\n";
    }
    object_behaviour_tester &operator=(object_behaviour_tester &&t)
    {
        strncpy(name, t.name, sizeof(name));
        output_warpper() << name << ": Move Assignment called.\n";
    }

    ~object_behaviour_tester() { output_warpper() << name << ": Destructor called.\n"; }

private:
    char name[NameSize];
};