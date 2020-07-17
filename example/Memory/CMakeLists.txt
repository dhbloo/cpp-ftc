set(SRC ${CMAKE_SOURCE_DIR}/include/FTC/Memory)

add_executable(Sample_Pmr_Usage ./pmr/PmrUsage.cpp ${SRC}/pmr/profile_resource.hpp)