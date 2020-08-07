#include "FTC/Mixin/Singleton.hpp"

#include "FTC/Debug/LifetimeTester.hpp"

#include <iostream>

using namespace ftc;

class A
    : public GlobalSingleton<A>
    , public LifetimeTester<>
{
public:
    A() : LifetimeTester<>("A") {}
    ~A() = default;

    int GetId() { return 'A'; }
};

class AWithCreator;
struct ACreator
{
    AWithCreator operator()();
};

class AWithCreator
    : public GlobalSingleton<AWithCreator, ACreator>
    , public LifetimeTester<>
{
public:
    AWithCreator(int idOffset) : LifetimeTester<>("AWithCreator"), idOffset(idOffset) {}
    ~AWithCreator() = default;

    int GetId() { return 'A' + idOffset; }

private:
    int idOffset;
};

AWithCreator ACreator::operator()()
{
    return AWithCreator(100);
}

class B
    : public StaticSingleton<B>
    , public LifetimeTester<>
{
public:
    B() : LifetimeTester<>("B") {}
    ~B() = default;

    int GetId() { return 'B'; }
};

class C
    : public DelayedStaticSingleton<C>
    , public LifetimeTester<>
{
public:
    C(int idOffset) : LifetimeTester<>("C"), idOffset(idOffset) {}
    ~C() = default;

    int GetId() { return 'C' + idOffset; }

private:
    int idOffset;
};

class D
    : public DynamicSingleton<D>
    , public LifetimeTester<>
{
public:
    D() : LifetimeTester<>("D") {}
    ~D() = default;

    int GetId() { return 'D'; }
};


class DWithCreator;
struct DCreator
{
    DWithCreator operator()();
};

class DWithCreator
    : public DynamicSingleton<DWithCreator, DCreator>
    , public LifetimeTester<>
{
public:
    DWithCreator(int idOffset) : LifetimeTester<>("DWithCreator"), idOffset(idOffset) {}
    ~DWithCreator() = default;

    int GetId() { return 'D' + idOffset; }

private:
    int idOffset;
};

DWithCreator DCreator::operator()()
{
    return DWithCreator(400);
}

int main()
{
    std::cout << "========== main() started ==========\n";
    std::cout << A::Get().GetId() << '\n';
    std::cout << AWithCreator::Get().GetId() << '\n';
    std::cout << B::Get().GetId() << '\n';
    std::cout << C::Get(1000).GetId() << '\n';
    std::cout << C::Get(2000).GetId() << '\n';
    std::cout << D::Get().GetId() << '\n';
    D::FreeInstance();
    std::cout << D::Get().GetId() << '\n';
    std::cout << DWithCreator::Get().GetId() << '\n';

    std::cout << "========== main() ended ==========\n";
}