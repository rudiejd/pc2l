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
set(CMAKE_CXX_FLAGS_DEBUG
	"${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer")

# Options
option(pc2l_BUILD_EXAMPLES "Build examples in the examples/ folder" OFF)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# add project's local CMake module files to CMAKE_MODULE_PATH
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

## External library dependencies
find_package(Threads REQUIRED)
find_package(MPI REQUIRED)

## Add all child directories
add_subdirectory(src)

# Only build examples or tests when explicitly requested
if(PC2L_ENABLE_TESTS)
	if (PC2L_DOWNLOAD_EXTERNALS)
		include(FetchContent)
		FetchContent_Declare(
						googletest
						URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
					)
		include(GoogleTest)
		set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Suppressing benchmark's tests" FORCE)
		FetchContent_Declare(googlebenchmark
						GIT_REPOSITORY https://github.com/google/benchmark.git
						GIT_TAG master) # need master for benchmark::benchmark
		FetchContent_MakeAvailable(googletest googlebenchmark)
	else()
		# We need MPI, GoogleTest, GoogleBenchmark for this project to work
		find_package(GTest REQUIRED)
		find_package(benchmark REQUIRED)
	endif()
	enable_testing()
	add_subdirectory(test)
	add_subdirectory(bench)
endif()

if (PC2L_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()

