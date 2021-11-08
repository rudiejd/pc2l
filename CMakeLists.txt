cmake_minimum_required(VERSION 3.14)
project(
	pc2l
	VERSION 0.1
	DESCRIPTION "PC2L: A Parallel and Cloud Computing Library"
	LANGUAGES CXX
	)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME) 
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)

	include(CTest)

endif()

include(FetchContent)

find_package(Threads REQUIRED)
add_subdirectory(src)
add_subdirectory(examples)
if (BUILD_TESTING)
	add_subdirectory(test)
endif()
