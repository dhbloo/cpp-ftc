set(SRC ${CMAKE_SOURCE_DIR}/src/Template)

add_executable(Sample_Variadic ./Variadic.cpp ${SRC}/Variadic.hpp)

add_executable(Sample_FunctionTraits ./FunctionTraits.cpp ${SRC}/Traits/FunctionTraits.hpp)