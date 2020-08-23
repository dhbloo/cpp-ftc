set(SRC ${SRC}/Traits)

add_executable(Sample_FunctionTraits ./FunctionTraits.cpp ${SRC}/FunctionTraits.hpp)
target_link_libraries(Sample_FunctionTraits FTC)