#include "FTC/Function/SmallFunction.hpp"

#include <gtest/gtest.h>

using namespace ftc;

TEST(SmallFunction, Size)
{
    EXPECT_EQ(sizeof(SmallFunction<void()>), 32);
    EXPECT_EQ(sizeof(SmallFunction<void(), 8>), 16);
    EXPECT_EQ(sizeof(SmallFunction<void(), 16>), 24);
    EXPECT_EQ(sizeof(SmallFunction<void(), 24>), 32);
    EXPECT_EQ(sizeof(SmallFunction<void(), 32>), 40);
    EXPECT_EQ(sizeof(SmallFunction<void(), 40>), 48);
    EXPECT_EQ(sizeof(SmallFunction<void(), 48>), 56);
    EXPECT_EQ(sizeof(SmallFunction<void(), 56>), 64);
}

int func(int x)
{
    return x + 42;
}

TEST(SmallFunction, FunctionPointer)
{
    SmallFunction<int(int)>     sf1(func);
    SmallFunction<int(int), 8>  sf2(func);
    SmallFunction<int(int), 16> sf3(func);
    SmallFunction<int(int), 32> sf4(func);

    SmallFunction<int(int)>     sf5 = func;
    SmallFunction<int(int), 8>  sf6 = func;
    SmallFunction<int(int), 16> sf7 = func;
    SmallFunction<int(int), 32> sf8 = func;
    for (int i = 0; i < 10; i++) {
        EXPECT_EQ(sf1(i), i + 42);
        EXPECT_EQ(sf2(i), i + 42);
        EXPECT_EQ(sf3(i), i + 42);
        EXPECT_EQ(sf4(i), i + 42);
        EXPECT_EQ(sf5(i), i + 42);
        EXPECT_EQ(sf6(i), i + 42);
        EXPECT_EQ(sf7(i), i + 42);
        EXPECT_EQ(sf8(i), i + 42);
    }
}

TEST(SmallFunction, Functor)
{
    struct functor
    {
        int operator()(int x) { return x + 42; }
    } f;

    SmallFunction<int(int)>     sf1(f);
    SmallFunction<int(int), 8>  sf2(f);
    SmallFunction<int(int), 16> sf3(f);
    SmallFunction<int(int), 32> sf4(f);

    SmallFunction<int(int)>     sf5 = f;
    SmallFunction<int(int), 8>  sf6 = f;
    SmallFunction<int(int), 16> sf7 = f;
    SmallFunction<int(int), 32> sf8 = f;
    for (int i = 0; i < 10; i++) {
        EXPECT_EQ(sf1(i), i + 42);
        EXPECT_EQ(sf2(i), i + 42);
        EXPECT_EQ(sf3(i), i + 42);
        EXPECT_EQ(sf4(i), i + 42);
        EXPECT_EQ(sf5(i), i + 42);
        EXPECT_EQ(sf6(i), i + 42);
        EXPECT_EQ(sf7(i), i + 42);
        EXPECT_EQ(sf8(i), i + 42);
    }
}

TEST(SmallFunction, Lambda)
{
    int  y = 42;
    auto f = [y](int x) { return x + y; };

    SmallFunction<int(int)>     sf1(f);
    SmallFunction<int(int), 8>  sf2(f);
    SmallFunction<int(int), 16> sf3(f);
    SmallFunction<int(int), 32> sf4(f);

    SmallFunction<int(int)>     sf5 = f;
    SmallFunction<int(int), 8>  sf6 = f;
    SmallFunction<int(int), 16> sf7 = f;
    SmallFunction<int(int), 32> sf8 = f;
    for (int i = 0; i < 10; i++) {
        EXPECT_EQ(sf1(i), i + 42);
        EXPECT_EQ(sf2(i), i + 42);
        EXPECT_EQ(sf3(i), i + 42);
        EXPECT_EQ(sf4(i), i + 42);
        EXPECT_EQ(sf5(i), i + 42);
        EXPECT_EQ(sf6(i), i + 42);
        EXPECT_EQ(sf7(i), i + 42);
        EXPECT_EQ(sf8(i), i + 42);
    }
}

TEST(SmallFunction, Assign)
{
    int  y = 42;
    auto f = [y](int x) { return x + y; };

    SmallFunction<int(int)> sf1(f);
    SmallFunction<int(int)> sf2(sf1);
    SmallFunction<int(int)> sf3(sf2);
    SmallFunction<int(int)> sf4 = sf3;
    SmallFunction<int(int)> sf5 = sf4;
    SmallFunction<int(int)> sf6;
    sf6 = sf5;

    for (int i = 0; i < 10; i++) {
        EXPECT_EQ(sf1(i), sf2(i));
        EXPECT_EQ(sf1(i), sf3(i));
        EXPECT_EQ(sf1(i), sf4(i));
        EXPECT_EQ(sf1(i), sf5(i));
        EXPECT_EQ(sf1(i), sf6(i));
    }
}

TEST(SmallFunction, OperatorBool)
{
    int  y = 42;
    auto f = [y](int x) { return x + y; };

    SmallFunction<int(int)> sf1(f);
    SmallFunction<int(int)> sf2;
    SmallFunction<int(int)> sf3 = sf1;

    EXPECT_EQ((bool)sf1, true);
    EXPECT_EQ((bool)sf2, false);
    EXPECT_EQ((bool)sf3, true);

    sf3 = nullptr;
    EXPECT_EQ((bool)sf3, false);
}

TEST(SmallFunction, OperatorEqual)
{
    int  y = 42;
    auto f = [y](int x) { return x + y; };

    SmallFunction<int(int)> sf1(f);
    SmallFunction<int(int)> sf2;
    SmallFunction<int(int)> sf3 = sf1;

    EXPECT_EQ(sf1 == nullptr, false);
    EXPECT_EQ(sf1 != nullptr, true);
    EXPECT_EQ(sf2 == nullptr, true);
    EXPECT_EQ(sf2 != nullptr, false);
    EXPECT_EQ(sf3 == nullptr, false);
    EXPECT_EQ(sf3 != nullptr, true);

    sf3 = nullptr;
    EXPECT_EQ(sf3 == nullptr, true);
    EXPECT_EQ(sf3 != nullptr, false);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}