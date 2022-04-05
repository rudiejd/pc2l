cmake_minimum_required(VERSION 3.14)
project(
	pc2l
	VERSION 0.1
	DESCRIPTION "PC2L: A Parallel and Cloud Computing Library"
	LANGUAGES CXX
	)
# Declare minimum C++ standard of 11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_VERBOSE_MAKEFILE ON)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

include(FetchContent)
FetchContent_Declare(
		googletest
		URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

FetchContent_MakeAvailable(googletest)

include(GoogleTest)
find_package(Threads REQUIRED)
enable_testing()
# function for adding a unit test that utilizes MPI
function(add_mpi_test name no_mpi_proc)
	include_directories(${MY_TESTING_INCLUDES})
	# My test are all called name_test.cpp
	add_executable(${name} test_${name}.cpp)
	# link with the pc2l library
	target_link_libraries(${name} pc2l_lib)
	target_link_libraries(${name} pc2l_lib)
	# Make sure to link MPI here too:
	target_link_libraries(${name} ${MPI_CXX_LIBRARY})
	# and link with testing library, discover tests??
	target_link_libraries(${name} gtest_main)
	# execute with mpi (e.g. mpirun -n 2, mpiexec, whatever)
	set(test_parameters "./${name}" ${MPIEXEC_NUMPROC_FLAG} ${no_mpi_proc})
	add_test(NAME ${name} COMMAND ${MPIEXEC} ${test_parameters})
endfunction(add_mpi_test)

add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(test)
