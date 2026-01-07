#!/bin/sh

for procs in {2..8}
do
    BENCHMARK_FORMAT=csv BENCHMARK_OUT="$1_$procs-procs.csv" mpirun -n $procs $1;
done

