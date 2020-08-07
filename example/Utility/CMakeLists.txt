set(SRC ${CMAKE_SOURCE_DIR}/include/FTC/Utility)

add_executable(Sample_Variadic ./Variadic.cpp ${SRC}/Variadic.hpp)
add_executable(Sample_OverloadSet ./OverloadSet.cpp ${SRC}/OverloadSet.hpp)