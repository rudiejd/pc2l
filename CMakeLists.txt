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

# Options
option(pc2l_BUILD_EXAMPLES "Build examples in the examples/ folder" OFF)

# if this is a debug build, enable our debug flags
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPC2L_DEBUG_MODE")
endif()

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# add project's local CMake module files to CMAKE_MODULE_PATH
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

## Internally provided libraries
find_package(Threads REQUIRED)

# Check and see if MPI is available
find_package(MPI)


## Configure header files to reflect the libarary availabilities
configure_file("${pc2l_SOURCE_DIR}/include/MPIHelper.h.in" "${pc2l_SOURCE_DIR}/include/MPIHelper.h")

## Externally provided libraries
include(FetchContent)
FetchContent_Declare(googletest
		GIT_REPOSITORY https://github.com/google/googletest.git
		GIT_TAG main)
#disable tests for google benchmark
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Suppressing benchmark's tests" FORCE)
FetchContent_Declare(googlebenchmark
		GIT_REPOSITORY https://github.com/google/benchmark.git
		GIT_TAG master) # need master for benchmark::benchmark

FetchContent_MakeAvailable(
		googletest
		googlebenchmark)

## Add all child directories
add_subdirectory(src)

# Only build examples when explicitly requested
if (pc2l_BUILD_EXAMPLES)
add_subdirectory(examples)
endif()

add_subdirectory(test)
add_subdirectory(bench)