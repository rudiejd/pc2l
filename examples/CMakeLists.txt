add_executable(big_vec big_vec.cpp)

target_compile_features(big_vec PRIVATE cxx_std_11)

target_link_libraries(big_vec PRIVATE pc2l_lib)

target_link_libraries(big_vec ${MPI_CXX_LIBRARY})


add_executable(big_matrix big_matrix.cpp Matrix.cpp)

target_compile_features(big_matrix PRIVATE cxx_std_11)

target_link_libraries(big_matrix PRIVATE pc2l_lib)

target_link_libraries(big_matrix ${MPI_CXX_LIBRARY})


# add_executable(std_matrix std_matrix.cpp STDMatrix.cpp)
#
# target_compile_features(std_matrix PRIVATE cxx_std_11)
#
# target_link_libraries(std_matrix PRIVATE pc2l_lib)
#
# target_link_libraries(std_matrix ${MPI_CXX_LIBRARY})
#
 #Web Cache example
#
# add_executable(web_cache web_cache.cpp Matrix.cpp ChildProcess.cpp HTTPFile.cpp)
#
# target_compile_features(web_cache PRIVATE cxx_std_11)
#
# target_link_libraries(web_cache pc2l_lib)

# find_package(Boost COMPONENTS system thread)
# # link boost, curl, curlpp for web cache example
# target_link_libraries(web_cache ${BOOST_LIBRARIES})
# target_link_libraries(web_cache Boost::thread)

# terrasort
# add_executable(terrasort terrasort.cpp)
#
# target_compile_features(terrasort PRIVATE cxx_std_11)
#
# target_link_libraries(terrasort pc2l_lib)
#
# # terrasort with std::vector
# add_executable(std_terrasort std_terrasort.cpp)
#

# minimal example
add_executable(minimal_example minimal_example.cpp)

target_compile_features(minimal_example PRIVATE cxx_std_11)

target_link_libraries(minimal_example pc2l_lib)
