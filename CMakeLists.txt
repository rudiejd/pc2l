cmake_minimum_required(VERSION 3.14)
project(
	pc2l
	VERSION 0.1
	DESCRIPTION "PC2L: A Parallel and Cloud Computing Library"
	LANGUAGES CXX
	)

##  Settings
# Declare minimum C++ standard of 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")

# Options
option(pc2l_BUILD_EXAMPLES "Build examples in the examples/ folder" OFF)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# add project's local CMake module files to CMAKE_MODULE_PATH
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

## External library dependencies
find_package(Threads REQUIRED)

# We need MPI for this project to work
find_package(MPI REQUIRED)

include(FetchContent)
find_package(GTest)
find_package(benchmark)
if (!GTest_Found)
	FetchContent_Declare(googletest
			GIT_REPOSITORY https://github.com/google/googletest.git
			GIT_TAG main)
	FetchContent_MakeAvailable(googletest)
endif()
if (!benchmark_FOUND)
	#disable tests for google benchmark
	set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Suppressing benchmark's tests" FORCE)
	FetchContent_Declare(googlebenchmark
			GIT_REPOSITORY https://github.com/google/benchmark.git
			GIT_TAG master) # need master for benchmark::benchmark
	FetchContent_MakeAvailable(googlebenchmark)
endif()

## Add all child directories
add_subdirectory(src)

# Only build examples when explicitly requested
if (pc2l_BUILD_EXAMPLES)
add_subdirectory(examples)
endif()

add_subdirectory(test)
add_subdirectory(bench)
