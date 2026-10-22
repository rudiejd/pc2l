#!/bin/sh

# Generates a matrix of results for a script (argument 1) strategy (argument 2) and every number of cores from 2 to 8

for procs in {2..8}
do
    BENCHMARK_FORMAT=csv BENCHMARK_OUT="$1-strategy-$2-procs-$procs.csv" mpirun -n $procs $1 $2;
done

