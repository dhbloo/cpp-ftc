# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# Now simply link against gtest or gtest_main as needed. Eg

enable_testing()
set(SRC ${CMAKE_SOURCE_DIR}/include/FTC)

function(add_ftc_test source_file)
	if(ARGV1)
		set(output_file ${ARGV1})
	else()
		string(REPLACE "/" "_" output_file ${source_file})
	endif()
	add_executable(Test_${output_file} ./${source_file}.cpp ${SRC}/${source_file}.hpp)
	target_link_libraries(Test_${output_file} FTC gtest_main)
	add_test(NAME ${output_file} COMMAND $<TARGET_FILE:Test_${output_file}>)
endfunction()

add_subdirectory(./Function)
