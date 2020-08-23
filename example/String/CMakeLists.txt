set(SRC ${SRC}/String)

add_executable(Sample_ConstString ./ConstString.cpp ${SRC}/ConstString.hpp)
target_link_libraries(Sample_ConstString FTC)