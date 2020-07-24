#include "FTC/Mixin/Auxiliary.hpp"

#include <type_traits>
#include <utility>

using namespace ftc;

class A : NonCopyable
{};

class B
{
    MAKE_NON_MOVABLE_POD(B)
};

class C : NonCopyMovable
{};

int main()
{
    A a1;
    // A a2 {a1};  // compile error
    A a3 {A {}};
    A a4 {std::move(a1)};

    static_assert(std::is_copy_constructible_v<A> == false);
    static_assert(std::is_copy_assignable_v<A> == false);
    static_assert(std::is_move_constructible_v<A> == true);
    static_assert(std::is_move_assignable_v<A> == true);

    B b1;
    B b2 {b1};
    B b3 {B {}};
    // B b4 {std::move(b1)};  // compile error

    static_assert(std::is_copy_constructible_v<B> == true);
    static_assert(std::is_copy_assignable_v<B> == true);
    static_assert(std::is_move_constructible_v<B> == false);
    static_assert(std::is_move_assignable_v<B> == false);

    C c1;
    // C c2 {c1};  // compile error
    C c3 {C {}};
    // C c4 {std::move(c1)};  // compile error

    static_assert(std::is_copy_constructible_v<C> == false);
    static_assert(std::is_copy_assignable_v<C> == false);
    static_assert(std::is_move_constructible_v<C> == false);
    static_assert(std::is_move_assignable_v<C> == false);
}