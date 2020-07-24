// Auxiliary Mixin
//
// Provides some useful miscellaneous mixin, such as Noncopyable, Nonmoveable

#pragma once

namespace ftc {

/// Non-copyable base class
///
/// Disable copy construction and copy assignment for one class
class NonCopyable
{
protected:
    NonCopyable()  = default;
    ~NonCopyable() = default;

    // Delete copy constructor & copy assignment operator
    NonCopyable(const NonCopyable &) = delete;
    NonCopyable &operator=(const NonCopyable &) = delete;

    // default move constructor & move assignment operator
    NonCopyable(NonCopyable &&) = default;
    NonCopyable &operator=(NonCopyable &&) = default;
};

/// Non-movable class boilerplate macro
///
/// Since there is no way to provide a base class for non-movable, a macro is the simplest
/// way to make a class non-movable. As when default move constructor is defined, implicit
/// constructor and implicit copy constructor is deleted, therefore here we define multiple
/// macros to provide these constructors/assignment operators at the same time.
#define MAKE_NON_MOVABLE(Class)                               MAKE_NON_MOVABLE_IMPL(Class, 0, 0)
#define MAKE_NON_MOVABLE_WITH_DEFAULT_CONSTRUCTOR(Class)      MAKE_NON_MOVABLE_IMPL(Class, 1, 0)
#define MAKE_NON_MOVABLE_WITH_DEFAULT_COPY_CONSTRUCTOR(Class) MAKE_NON_MOVABLE_IMPL(Class, 0, 1)
#define MAKE_NON_MOVABLE_POD(Class)                           MAKE_NON_MOVABLE_IMPL(Class, 1, 1)
#define MAKE_NON_MOVABLE_IMPL(Class, WithDefaultConstructor, WithDefaultCopy) \
public:                                                                       \
    Class(Class &&) = delete;                                                 \
    Class &operator=(Class &&) = delete;                                      \
    __ADD_DEFAULT_CONSTRUCTOR_##WithDefaultConstructor(Class)                 \
        __ADD_DEFAULT_COPY_##WithDefaultCopy(Class)

#define __ADD_DEFAULT_CONSTRUCTOR_0(Class)
#define __ADD_DEFAULT_CONSTRUCTOR_1(Class) Class() = default;
#define __ADD_DEFAULT_COPY_0(Class)
#define __ADD_DEFAULT_COPY_1(Class) \
    Class(const Class &) = default; \
    Class &operator=(const Class &) = default;

/// Non-copyable and Non-movable base class
///
/// Disable copy/move construction and copy/move assignment for one class
class NonCopyMovable
{
protected:
    NonCopyMovable()  = default;
    ~NonCopyMovable() = default;

    // Delete copy constructor & copy assignment operator
    NonCopyMovable(const NonCopyMovable &) = delete;
    NonCopyMovable &operator=(const NonCopyMovable &) = delete;

    // Delete move constructor & move assignment operator
    NonCopyMovable(NonCopyMovable &&) = delete;
    NonCopyMovable &operator=(NonCopyMovable &&) = delete;
};

}  // namespace ftc