set(SRC ${SRC}/Memory)

add_executable(Sample_PmrUsage ./pmr/PmrUsage.cpp ${SRC}/pmr/ProfileResource.hpp)
target_link_libraries(Sample_PmrUsage FTC)