set(SRC ${SRC}/Mixin)

add_executable(Sample_AuxiliaryMixin ./Auxiliary.cpp ${SRC}/Auxiliary.hpp)
target_link_libraries(Sample_AuxiliaryMixin FTC)

add_executable(Sample_SingletonMixin ./Singleton.cpp ${SRC}/Singleton.hpp)
target_link_libraries(Sample_SingletonMixin FTC)