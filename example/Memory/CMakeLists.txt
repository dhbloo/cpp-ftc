set(SRC ${CMAKE_SOURCE_DIR}/include/FTC/Memory)

add_executable(Sample_PmrUsage ./pmr/PmrUsage.cpp ${SRC}/pmr/ProfileResource.hpp)