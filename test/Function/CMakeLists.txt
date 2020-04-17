set(SRC ${CMAKE_SOURCE_DIR}/src/Function)

add_executable(Test_StaticFunction ./StaticFunction.cpp ${SRC}/StaticFunction.hpp)
target_link_libraries(Test_StaticFunction gtest_main)
add_test(NAME StaticFunctionTest COMMAND Test_StaticFunction)