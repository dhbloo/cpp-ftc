set(SRC ${CMAKE_SOURCE_DIR}/include/FTC/Function)

add_executable(Test_SmallFunction ./SmallFunction.cpp ${SRC}/SmallFunction.hpp)
target_link_libraries(Test_SmallFunction gtest_main)
add_test(NAME SmallFunctionTest COMMAND Test_SmallFunction)