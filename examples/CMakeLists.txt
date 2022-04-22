add_executable(big_vec big_vec.cpp)

target_compile_features(big_vec PRIVATE cxx_std_11)

target_link_libraries(big_vec PRIVATE pc2l_lib)

target_link_libraries(big_vec ${MPI_CXX_LIBRARY})


add_executable(big_matrix big_matrix.cpp Matrix.cpp)

target_compile_features(big_matrix PRIVATE cxx_std_11)

target_link_libraries(big_matrix PRIVATE pc2l_lib)

target_link_libraries(big_matrix ${MPI_CXX_LIBRARY})


add_executable(std_matrix std_matrix.cpp STDMatrix.cpp)

target_compile_features(std_matrix PRIVATE cxx_std_11)

target_link_libraries(std_matrix PRIVATE pc2l_lib)

target_link_libraries(std_matrix ${MPI_CXX_LIBRARY})

# Web Cache example

#add_executable(web_cache web_cache.cpp Matrix.cpp ChildProcess.cpp)
#
#target_compile_features(web_cache PRIVATE cxx_std_11)
#
#target_link_libraries(web_cache pc2l_lib)
#
#
#
#find_package(CURL REQUIRED)
#
#if (CURL_FOUND)
#    message(STATUS "Found curl version: ${CURL_VERSION_STRING}")
#    message(STATUS "Using curl version: ${CURL_INCLUDE_DIRS}")
#    message(STATUS "Using curl libraries: ${CURL_LIBRARIES}\n")
#    list(APPEND pc2l_INCLUDE_DIRS ${CURL_INCLUDE_DIRS})
#else()
#    message(FATAL_ERROR "Could not find curl.")
#endif()
#
#
#message(STATUS "Looking for curlpp...")
#find_package(CURLPP REQUIRED)
#
#if (CURLPP_FOUND)
#    message(STATUS "Found curlpp version: ${CURLPP_VERSION}")
#    message(STATUS "Using curlpp include dir: ${CURLPP_INCLUDE_DIR}")
#    message(STATUS "Using curlpp libraries: ${CURLPP_LIBRARY}\n")
#    list(APPEND pc2l_INCLUDE_DIRS ${CURLPP_INCLUDE_DIR})
#else()
#    message(FATAL_ERROR "Could not find curlpp.")
#endif()
#find_package(Boost 1.40.0 REQUIRED system)
## link boost, curl, curlpp for web cache example
#target_link_libraries(web_cache ${BOOST_LIBRARIES})
#target_link_libraries(web_cache ${CURL_LIBRARIES})
#target_link_libraries(web_cache ${CURLPP_LIBRARY})
