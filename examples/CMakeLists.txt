add_executable(big_vec big_vec.cpp)

target_compile_features(big_vec PRIVATE cxx_std_11)

target_link_libraries(big_vec PRIVATE pc2l_lib)
