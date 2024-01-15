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
// Authors:   JD Rudie                            rudiejd@miamioh.edu
//---------------------------------------------------------------------

#include <iostream>
#include <random>
#include "Environment.h"


class MRUTest : public ::testing::Test {

};

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    auto& pc2l = pc2l::System::get();
    pc2l.setCacheSize(3 * (sizeof(pc2l::Message) + 8 * sizeof(int)));
    pc2l.initialize(argc, argv);
    pc2l.start(pc2l::System::MostRecentlyUsed);
    auto env = new PC2LEnvironment();
    env->argc = argc;
    env->argv = argv;
    ::testing::AddGlobalTestEnvironment(env);
    return RUN_ALL_TESTS();
}

TEST_F(MRUTest, test_mru_caching) {
    auto& pc2l = pc2l::System::get();
    const int listSize = 100;
    pc2l::Vector<int, 8 * sizeof(int)> intVec = createRangeIntVec(listSize);
    // cache should now be the MRU block, the 2nd LRU block, and the LRU block
    // MRU order 12 1 0
    ASSERT_NE(pc2l.cacheManager().getBlock(intVec.dsTag, 12, true), nullptr);
    ASSERT_NE(pc2l.cacheManager().getBlock(intVec.dsTag, 0, true), nullptr);
    ASSERT_NE(pc2l.cacheManager().getBlock(intVec.dsTag, 1, true), nullptr);


    // now put the eleven block back in cache
    // MRU order: 12 11 10
    intVec[90] = 1;
    // MRU order: 0 11 10
    // 12 should be removed
    ASSERT_EQ(pc2l.cacheManager().getBlock(intVec.dsTag, 12, true), nullptr);
    intVec[9] = 1;
    // New MRU order: 11 1 0
    ASSERT_NE(pc2l.cacheManager().getBlock(intVec.dsTag, 0, true), nullptr);
    ASSERT_NE(pc2l.cacheManager().getBlock(intVec.dsTag, 1, true), nullptr);
    ASSERT_NE(pc2l.cacheManager().getBlock(intVec.dsTag, 11, true), nullptr);
}


