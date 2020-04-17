#pragma once
#include <cstring>
#include <iostream>

namespace cpppg {

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

template <typename OutputWarpper = StdoutWrapper, std::size_t NameSize = 8>
class ObjBehaviourTester
{
public:
    ObjBehaviourTester(const char *_name = "Unnamed")
    {
        strncpy(name, _name, sizeof(name));
        OutputWarpper() << name << ": Default constructor called.\n";
    }

    ObjBehaviourTester(const ObjBehaviourTester &t)
    {
        strncpy(name, t.name, sizeof(name));
        OutputWarpper() << name << ": Copy constructor called.\n";
    }
    ObjBehaviourTester(ObjBehaviourTester &&t)
    {
        strncpy(name, t.name, sizeof(name));
        OutputWarpper() << name << ": Move constructor called.\n";
    }

    ObjBehaviourTester &operator=(const ObjBehaviourTester &t)
    {
        strncpy(name, t.name, sizeof(name));
        OutputWarpper() << name << ": Copy Assignment called.\n";
    }
    ObjBehaviourTester &operator=(ObjBehaviourTester &&t)
    {
        strncpy(name, t.name, sizeof(name));
        OutputWarpper() << name << ": Move Assignment called.\n";
    }

    ~ObjBehaviourTester() { OutputWarpper() << name << ": Destructor called.\n"; }

private:
    char name[NameSize];
};

}  // namespace cpppg