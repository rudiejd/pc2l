#ifndef BIG_VEC_CPP
#define BIG_VEC_CPP

//---------------------------------------------------------------------
//  ____
// |  _ \    This file is part of  PC2L:  A Parallel & Cloud Computing
// | |_) |   Library <http://www.pc2lab.cec.miamioh.edu/pc2l>. PC2L is
// |  __/    free software: you can  redistribute it and/or  modify it
// |_|       under the terms of the GNU  General Public License  (GPL)
//           as published  by  the   Free  Software Foundation, either
//           version 3 (GPL v3), or  (at your option) a later version.
//
//   ____    PC2L  is distributed in the hope that it will  be useful,
//  / ___|   but   WITHOUT  ANY  WARRANTY;  without  even  the IMPLIED
// | |       WARRANTY of  MERCHANTABILITY  or FITNESS FOR A PARTICULAR
// | |___    PURPOSE.
//  \____|
//            Miami University and  the PC2Lab development team make no
//            representations  or  warranties  about the suitability of
//  ____      the software,  either  express  or implied, including but
// |___ \     not limited to the implied warranties of merchantability,
//   __) |    fitness  for a  particular  purpose, or non-infringement.
//  / __/     Miami  University and  its affiliates shall not be liable
// |_____|    for any damages  suffered by the  licensee as a result of
//            using, modifying,  or distributing  this software  or its
//            derivatives.
//
//  _         By using or  copying  this  Software,  Licensee  agree to
// | |        abide  by the intellectual  property laws,  and all other
// | |        applicable  laws of  the U.S.,  and the terms of the  GNU
// | |___     General  Public  License  (version 3).  You  should  have
// |_____|    received a  copy of the  GNU General Public License along
//            with MUSE.  If not,  you may  download  copies  of GPL V3
//            from <http://www.gnu.org/licenses/>.
//
// --------------------------------------------------------------------
// Authors:   JD Rudie          rudiejd@miamioh.edu
//---------------------------------------------------------------------
#include <algorithm>
#include <iostream>
#include <numeric>
#include <pc2l.h>
using ull = unsigned long long;

int main(int argc, char *argv[]) {
  // Boilerplate code to get MPI up and running
  auto &pc2l = pc2l::System::get();
  pc2l.initialize(argc, argv);
  pc2l.start();
  ull num = strtoull(argv[1], NULL, 0);

  // initialize a vector of type unsigned long long filled with the number
  // specified by 2nd command line argument
  pc2l::Vector<ull> vec(num);

  // fill vector with values from 1 to n
  std::iota(vec.begin(), vec.end(), 1);

  // every number that does not have a 3 or 5 as a factor is set to 0
  std::replace_if(
      vec.begin(), vec.end(), [](auto i) { return !((i % 3) || (i % 5)); }, 0);

  // sum all elements in the vector
  auto total = std::accumulate(vec.begin(), vec.end(), 0ULL);

  std::cout << "The total is " << total << std::endl;

  // Boilerplate to shut down MPI (could be included in other file
  pc2l.stop();
  pc2l.finalize();
}

#endif
