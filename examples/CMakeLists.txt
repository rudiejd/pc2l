add_executable(big_vec big_vec.cpp)

target_compile_features(big_vec PRIVATE cxx_std_11)

target_link_libraries(big_vec PRIVATE pc2l_lib)

target_link_libraries(big_vec ${MPI_CXX_LIBRARY})


add_executable(big_matrix big_matrix.cpp Matrix.cpp)

target_compile_features(big_matrix PRIVATE cxx_std_11)

target_link_libraries(big_matrix PRIVATE pc2l_lib)

target_link_libraries(big_matrix PRIVATE pc2l_lib)

target_link_libraries(big_matrix ${MPI_CXX_LIBRARY})


add_executable(std_matrix std_matrix.cpp STDMatrix.cpp)

target_compile_features(std_matrix PRIVATE cxx_std_11)

target_link_libraries(std_matrix PRIVATE pc2l_lib)

target_link_libraries(std_matrix PRIVATE pc2l_lib)

target_link_libraries(std_matrix ${MPI_CXX_LIBRARY})
