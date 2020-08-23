set(SRC ${SRC}/Utility)

add_executable(Sample_Variadic ./Variadic.cpp ${SRC}/Variadic.hpp)
target_link_libraries(Sample_Variadic FTC)

add_executable(Sample_OverloadSet ./OverloadSet.cpp ${SRC}/OverloadSet.hpp)
target_link_libraries(Sample_Variadic FTC)