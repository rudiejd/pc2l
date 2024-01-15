# Overview
The Parallel & Cloud Computing Library (PC2L) is a simple library containing 
implementations of distributed data structures  intended for use in a 
clustered computing environment with MPI. This was developed in Dr. DJ Rao's
Lab at Miami University. You can read more about the lab [here](http://www.pc2lab.cec.miamioh.edu/pc2l>)


# Compiling

```sh
git clone https://github.com/rudiejd/pc2l.git
cd pc2l
mkdir build
cd build
cmake ..
```

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

