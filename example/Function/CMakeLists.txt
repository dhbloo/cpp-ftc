set(SRC ${SRC}/Function)

add_executable(Sample_SmallFunction ./SmallFunction.cpp ${SRC}/SmallFunction.hpp)
target_link_libraries(Sample_SmallFunction FTC)
