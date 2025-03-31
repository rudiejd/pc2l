include(GoogleTest)
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
	add_test(NAME test_${name}
		COMMAND mpirun -n "${no_mpi_proc}" "../bin/test_${name}")
endfunction(add_mpi_test)

add_mpi_test(vector 4)
add_mpi_test(map 4)
add_mpi_test(lfu 4)
add_mpi_test(mru 4)
add_mpi_test(plru 4)
