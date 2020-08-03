set(SRC ${CMAKE_SOURCE_DIR}/include/FTC/Mixin)

add_executable(Sample_AuxiliaryMixin ./Auxiliary.cpp ${SRC}/Auxiliary.hpp)
add_executable(Sample_SingletonMixin ./Singleton.cpp ${SRC}/Singleton.hpp)