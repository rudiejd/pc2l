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
// Authors:   JD Rudie                             rudiejd@miamioh.edu
//---------------------------------------------------------------------

#include <iostream>
#include <gtest/gtest.h>
#include <pc2l.h>

int main(int argc, char *argv[]) {
    for (int i = 0; i < argc; i++) std::cout << argv[i] << std::endl;
    ::testing::InitGoogleTest(&argc, argv);
    auto& pc2l = pc2l::System::get();
    pc2l.initialize(argc, argv);
    return RUN_ALL_TESTS();
}

// Google Test structure for vector class
class VectorTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto& pc2l = pc2l::System::get();
        pc2l.start();
    }

//    void TearDown() override {
//        auto& pc2l = pc2l::System::get();
//        pc2l.stop();
//        pc2l.finalize();
//    }
};

TEST_F(VectorTest, test_insert_int) {
    // Test inserting 100 integers
    pc2l::Vector<int> intVec;
    for (int i = 0; i < 100; i++) {
        ASSERT_NO_THROW(intVec.insert(i, i));
    }
    // intVec.clear();
}

TEST_F(VectorTest, test_at) {
    pc2l::Vector<int> intVec;
    for (int i = 0; i < 100; i++) {
        ASSERT_NO_THROW(intVec.insert(i, i));
    }
    // check to see if data is correct (tests deserializtion)
    for (int i = 0; i < 100; i++) {
        ASSERT_EQ(i, intVec.at(i));
    }
}

TEST_F(VectorTest, test_lru_caching) {
    pc2l::Vector<int> intVec;
    // set block size to 5 integers
    for (int i = 0; i < 100; i++) {
        ASSERT_NO_THROW(intVec.insert(i, 100));
    }
    auto& pc2l = pc2l::System::get();
    // cache should contain 3 blocks 85-90, 90-95, 95-100
    ASSERT_EQ(pc2l.cacheManager().managerCache().size(), 3);
    // cache should now be the last 3 blocks inserted
    // front of cache is the last block, rear of cache is the third-last (LRU)
    // now retrieve another block
    intVec.at(0);
    // cache should be size 3
    ASSERT_EQ(pc2l.cacheManager().managerCache().size(), 3);
    // the third to last block should be removed from cache (contains 85-90)
    // the 0-5 block should be in the cache
}

/*TEST_F(VectorTest, test_caching) {
    // Test caching on the vector
    for (int i = 0; i < 100; i++) {
        ASSERT_NO_THROW(intVec.insert(i, i));
    }
}*/
