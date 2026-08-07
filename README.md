# Overview
The Parallel & Cloud Computing Library (PC2L) is a simple library containing 
implementations of distributed data structures  intended for use in a 
clustered computing environment with MPI. This was developed in Dr. DJ Rao's
Lab at Miami University. You can read more about the lab [here](http://www.pc2lab.cec.miamioh.edu/pc2l>)


# Development
You must be running linux, and you must have an MPI compiler installed 
and linked in your /usr/bin folder as `/usr/bin/mpicxx`. Many MPI 
distributions have this as default behavior, but you should double check. 
Python is also necessary if you wish to graph the benchmark outputs.

For your convenience, a `mise.toml` file is provided which will allow you to
easily install the necessary version `gcc`, `cmake`, and `python`.

## Dependencies
- Google's [Benchmark library](https://github.com/google/benchmark) for framework-level benchmarks 
- [GoogleTest](https://github.com/google/googletest) for running tests

To download all dependencies, you can set the build option `PC2L_DOWNLOAD_EXTERNALS=true`

To disable tests, set `PC2L_ENABLE_TESTS=false`. If you aren't building tests,
need either of the external dependencies. You should disable tests if you are using
PC2L as a library.

## Example
```sh
git clone https://github.com/rudiejd/pc2l.git
cd pc2l
cmake -Bbuild -DPC2L_DOWNLOAD_EXTERNALS=true -DPC2L_ENABLE_TESTS=true -DPC2L_BUILD_EXAMPLES=true -DCMAKE_CXX_COMPILER=mpicxx -DCMAKE_BUILD_TYPE=Debug  
```

## Running the tests
```sh  
# functional tests
mpirun -n 10 ./bin/test_vector

# load tests/benchmarks
mpirun -n 10 /bin/bench_vector
```

## Debugging
You can find general approaches for debugging MPI applications in the [OpenMPI documentation](https://www.open-mpi.org/faq/?category=debugging).

One of the simplest ways to debug PC2L is by running one GDB instance for each process in separate terminal windows. For example:
```sh
mpirun -n 2 alacritty -e gdb ./build/bin/test_vector
```

Replace `alacritty` with your terminal of choice.

# Running programs that utilize PC2L

Programs that utilize PC2L must be run using MPI via `mpirun` or a 
similar shell program. Failing to do so will result in undefined
behavior.

# License

A Parallel & Cloud Computing Library (PC2L) is free software: you can
redistribute it and/or modify it under the terms of the GNU General
Public License (GPL) as published by the Free Software Foundation,
either version 3 (GPL v3), or (at your option) a later version.  You
should have received a copy of the GNU General Public License along
with PC2L.  If not, you may download copies of GPL V3 from
<http://www.gnu.org/licenses/>.


# Disclaimer

PC2L  is distributed in the hope that it will  be useful,
but   WITHOUT  ANY  WARRANTY;  without  even  the IMPLIED
WARRANTY of  MERCHANTABILITY  or FITNESS FOR A PARTICULAR
PURPOSE.

Miami University and  the PC2Lab development team make no
representations  or  warranties  about the suitability of
the software,  either  express  or implied, including but
not limited to the implied warranties of merchantability,
fitness  for a  particular  purpose, or non-infringement.
Miami  University and  its affiliates shall not be liable
for any damages  suffered by the  licensee as a result of
using, modifying,  or distributing  this software  or its
derivatives.

By using or  copying  this  Software,  Licensee  agree to
abide  by the intellectual  property laws,  and all other
applicable  laws of  the U.S.,  and the terms of the  GNU
General  Public  License  (version 3).

