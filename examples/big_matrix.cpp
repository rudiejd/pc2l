#ifndef BIG_MATRIX_CPP
#define BIG_MATRIX_CPP

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

#include <iostream>
#include <pc2l.h>
#include "Matrix.h"

int main(int argc, char *argv[]) {
    auto start = clock();
    auto& pc2l = pc2l::System::get();
    unsigned long long size = strtoull(argv[1], NULL, 0);
    pc2l.initialize(argc, argv);
    // 50 block cache
    pc2l.setCacheSize(50 * 100 * sizeof(double));
    pc2l.start();
    Matrix m1(size, size, 1);
    Matrix m2(size, size, 1);
    std::cout << "setup took " << ((clock() - start) * 1000000000) / CLOCKS_PER_SEC << "ns" << std::endl;

    start = clock();

    Matrix res = m1.dot(m2);

    std::cout << "dot took " << ((clock() - start) * 1000000000) / CLOCKS_PER_SEC << "ns" << std::endl;

    // Wind-up
    pc2l.stop();
    pc2l.finalize();
}

#endif
