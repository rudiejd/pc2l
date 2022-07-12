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

const unsigned int PC2LEnvironment::blockSize;
const unsigned int PC2LEnvironment::cacheSize;
const unsigned int PC2LEnvironment::blocksInCache;


class VectorTest : public ::testing::Test {

};

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    auto env = new PC2LEnvironment();
    env->argc = argc;
    env->argv = argv;
    ::testing::AddGlobalTestEnvironment(env);
    return RUN_ALL_TESTS();
}
TEST_F(VectorTest, test_insert_int) {
    // Test inserting 100 integers
    pc2l::Vector<int, 8 * sizeof(int)> intVec;
    for (int i = 0; i < 100; i++) {
        ASSERT_NO_THROW(intVec.insert(i, i));
    }

    // Insert at the beginning - everything should be moved over by 1
    intVec.insert(0, 0);
    ASSERT_EQ(intVec.at(0), 0);
    ASSERT_EQ(intVec.size(), 101);
    for (size_t i =  1; i < intVec.size(); i++) {
       ASSERT_EQ(i-1, intVec.at(i));
    }

    // Insert at index 50 - everything after 50 should be moved, before 50 should stay same
    intVec.insert(50, 0);
    ASSERT_EQ(intVec.at(0), 0);
    for (size_t i =  1; i < 50; i++) {
        ASSERT_EQ(i-1, intVec.at(i));
    }
    for (size_t i =  51; i < intVec.size(); i++) {
        ASSERT_EQ(i-2, intVec.at(i));
    }
    // intVec.clear();
}

TEST_F(VectorTest, test_at) {
    pc2l::Vector<int, 8 * sizeof(int)> intVec;
    for (int i = 0; i < 100; i++) {
        intVec.insert(i, i);
    }
    // check to see if data is correct (tests deserializtion)
    for (int i = 0; i < 100; i++) {
        ASSERT_EQ(i, intVec.at(i));
    }
}

TEST_F(VectorTest, test_operatorbrack) {
    pc2l::Vector<int, 8 * sizeof(int)> intVec;
    for (int i = 0; i < 100; i++) {
        intVec.push_back(i);
    }
    for (int i = 0; i < 100; i++) {
        ASSERT_TRUE(intVec[static_cast<size_t>(i)] == i);
    }
}

// Test the custom iterator for our vector class
TEST_F(VectorTest, test_iterator) {
    pc2l::Vector<int, 8 * sizeof(int)> intVec;
    for (int i = 0; i < 100; i++) {
        intVec.insert(i, i);
    }
    int i = 0;
    // check to see if data is correct (tests deserializtion)
    for (auto e : intVec) {
        ASSERT_EQ(e, i);
        ++i;
    }
}

TEST_F(VectorTest, test_lru_caching) {
    auto& pc2l = pc2l::System::get();
    pc2l::Vector<int, 8 * sizeof(int)> intVec;
    const int listSize = 100;
    for (int i = 0; i < listSize; i++) {
        ASSERT_NO_THROW(intVec.insert(i, i));
    }
    // cache should now be the last 3 blocks inserted
    ASSERT_NE(pc2l.cacheManager().getBlock(intVec.dsTag, 12), nullptr);
    ASSERT_NE(pc2l.cacheManager().getBlock(intVec.dsTag, 11), nullptr);
    ASSERT_NE(pc2l.cacheManager().getBlock(intVec.dsTag, 10), nullptr);

    // front of cache is the last block, rear of cache is the third-last (LRU)
    // now retrieve another block
    intVec.at(0);
    ASSERT_EQ(pc2l.cacheManager().managerCache().size(), 3);
    ASSERT_EQ(pc2l.cacheManager().getBlock(intVec.dsTag, 10), nullptr);
    ASSERT_NE(pc2l.cacheManager().getBlock(intVec.dsTag, 0), nullptr);
}

TEST_F(VectorTest, test_delete) {
    auto& pc2l = pc2l::System::get();
    pc2l::Vector<int, 8 * sizeof(int)> intVec;
    // this produces 20 blocks of integers
    for (size_t i = 0; i < 100; i++) {
        ASSERT_NO_THROW(intVec.insert(i, i));
    }
    // delete at index 42
    ASSERT_NO_THROW(intVec.erase(42));
    // size should be 99
    ASSERT_EQ(intVec.size(), 99);
    for (size_t i = 0; i < 42; i++) {
        // up to modified index it should be the same
        ASSERT_EQ(intVec.at(i), i);
    }
    for (size_t i = 42; i < intVec.size(); i++) {
        // every other value should be one bigger than it was
        ASSERT_EQ(intVec.at(i), i+1);
    }
}

TEST_F(VectorTest, test_std_find) {
    auto& pc2l = pc2l::System::get();
    pc2l::Vector<int, 8 * sizeof(int)> intVec;
    // this produces 20 blocks of integers
    for (size_t i = 0; i < 100; i++) {
        ASSERT_NO_THROW(intVec.insert(i, i));
    }
    ASSERT_EQ(*std::find(intVec.begin(), intVec.end(), 50), 50);
    ASSERT_EQ(*std::find(intVec.begin(), intVec.end(), 0), 0);
    ASSERT_EQ(*std::find(intVec.begin(), intVec.end(), 99), 99);
}

TEST_F(VectorTest, test_std_sort) {
    pc2l::Vector<int, 8 * sizeof(int)> intVec;
    pc2l::Vector<int, 8 * sizeof(int)> intVecSorted;

    // push 100 random numbers 0 - 99
    for (unsigned int i = 0; i < 100; i++) {
        intVec.push_back(rand() % 100);
    }

    std::sort(intVec.begin(), intVec.end());

    // assert sorted order
    for (unsigned int i = 1; i < 100; i++) {
        std::cout << "the sorted value at " << i << " is " << intVec[i] << std::endl;
        ASSERT_TRUE(intVec.at(i) >= intVec.at(i - 1));
    }
}

TEST_F(VectorTest, test_sort) {
    pc2l::Vector<int, 8 * sizeof(int)> intVec;

    // push 100 random numbers 0 - 99
    for (unsigned int i = 0; i < 100; i++) {
        intVec.push_back(rand() % 100);
    }

    intVec.sort();

    // assert sorted order
    for (unsigned int i = 1; i < 100; i++) {
        ASSERT_TRUE(intVec.at(i) >= intVec.at(i - 1));
    }
}

/*TEST_F(VectorTest, test_caching) {
    // Test caching on the vector
    for (int i = 0; i < 100; i++) {
        ASSERT_NO_THROW(intVec.insert(i, i));
    }
}*/
