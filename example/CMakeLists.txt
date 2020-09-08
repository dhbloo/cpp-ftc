set(SRC ${CMAKE_SOURCE_DIR}/include/FTC)

function(add_ftc_example source_file)
	if(ARGV1)
		set(output_file ${ARGV1})
	else()
		string(REPLACE "/" "_" output_file ${source_file})
	endif()
	add_executable(Sample_${output_file} ./${source_file}.cpp ${SRC}/${source_file}.hpp)
	target_link_libraries(Sample_${output_file} FTC)
endfunction()


add_subdirectory(./Function)
add_subdirectory(./Memory)
add_subdirectory(./Mixin)
add_subdirectory(./String)
add_subdirectory(./Traits)
add_subdirectory(./Utility)

