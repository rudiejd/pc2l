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
// Authors:   Dhananjai M. Rao          raodm@miamioh.edu
//---------------------------------------------------------------------

#include <iostream>
#include <pc2l.h>

int main(int argc, char *argv[]) {
    auto& pc2l = pc2l::System::get();
    pc2l.initialize(argc, argv);
    pc2l.start();

    // Do some testing here.
    std::cout << "world size " << pc2l.worldSize() << std::endl;
    pc2l::Vector<char*> v;
    char* data = (char*) "String test";
    for (int i = 0; i < 100; i++) {
        v.insert(i, data);
    }
    for (int i = 0; i < 100; i++) {
        std::cout << "at " << i << " " << v.at(i) << std::endl;
    }

    // delete even indices
    for (int i = 0; i < 10; i++) {
        v.erase(0);
    }

    std::cout << "first ten removed" << std::endl;

    for (int i = 0; i < v.size(); i++) {
        std::cout << "at " << i << " " << v.at(i) << std::endl;
    }

    // Wind-up
    pc2l.stop();
    pc2l.finalize();
}

#endif
