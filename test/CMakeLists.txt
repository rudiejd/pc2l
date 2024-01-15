include(GoogleTest)
enable_testing()
# function for adding a unit test that utilizes MPI
function(add_mpi_test name no_mpi_proc)
    include_directories(${MY_TESTING_INCLUDES})
    # My test are all called name_test.cpp
    add_executable(test_${name} test_${name}.cpp)
    # link with the pc2l library
    target_link_libraries(test_${name} pc2l_lib)
    # Make sure to link MPI here too:
    target_link_libraries(test_${name} ${MPI_CXX_LIBRARY})
    # and link with testing library, discover tests??
    target_link_libraries(test_${name} gtest gtest_main)
    # execute with mpi (e.g. mpirun -n 2, mpiexec, whatever)
    set(test_parameters "./test_${name}" ${MPIEXEC_NUMPROC_FLAG} ${no_mpi_proc})
    add_test(NAME test_${name} COMMAND ${MPIEXEC} ${test_parameters})
endfunction(add_mpi_test)

add_mpi_test(vector 2)
add_mpi_test(map 2)
add_mpi_test(lfu 2)
add_mpi_test(mru 2)
add_mpi_test(plru 2)
