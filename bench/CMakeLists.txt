# add a benchmark
function(add_pc2l_bench name)
    # benches all called bench_name
    add_executable(bench_${name} bench_${name}.cpp)
    # link with the pc2l library
    target_link_libraries(bench_${name} pc2l_lib)
    # Make sure to link MPI here too:
    target_link_libraries(bench_${name} ${MPI_CXX_LIBRARY})
    # and link with bench library
    target_link_libraries(bench_${name} benchmark::benchmark)
endfunction(add_pc2l_bench)

add_pc2l_bench(vector)
add_pc2l_bench(std_vector)


