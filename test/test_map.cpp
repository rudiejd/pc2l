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

#include "Environment.h"
#include <iostream>

class UnorderedMapTest : public ::testing::Test {};

int main(int argc, char *argv[]) {
  for (int i = 0; i < argc; i++)
    std::cout << argv[i] << std::endl;
  ::testing::InitGoogleTest(&argc, argv);
  auto &pc2l = pc2l::System::get();
  auto env = new PC2LEnvironment();
  pc2l.setCacheSize(3 * (sizeof(pc2l::Message) + 4096));
  env->argc = argc;
  env->argv = argv;
  ::testing::AddGlobalTestEnvironment(env);
  return RUN_ALL_TESTS();
}

TEST_F(UnorderedMapTest, test_insert) {
  pc2l::Map<std::array<char, 10>, int> map;
  std::array<char, 10> appleArr = {'a', 'p', 'p', 'l', 'e'};
  ASSERT_NO_THROW(map[appleArr] = 1);
  std::array<char, 10> bananaArr = {'b', 'a', 'n', 'a', 'n', 'a'};
  ASSERT_NO_THROW(map[bananaArr] = 2);
  std::array<char, 10> carrotArr = {'c', 'a', 'r', 'r', 'o', 't'};
  ASSERT_NO_THROW(map[carrotArr] = 3);
}

TEST_F(UnorderedMapTest, test_at) {
  pc2l::Map<std::array<char, 10>, int> map;
  std::array<char, 10> appleArr = {'a', 'p', 'p', 'l', 'e'};
  map[appleArr] = 1;
  ASSERT_EQ(map[appleArr], 1);
  std::array<char, 10> bananaArr = {'b', 'a', 'n', 'a', 'n', 'a'};
  map[bananaArr] = 2;
  ASSERT_EQ(map[bananaArr], 2);
  std::array<char, 10> carrotArr = {'c', 'a', 'r', 'r', 'o', 't'};
  map[carrotArr] = 3;
  ASSERT_EQ(map[carrotArr], 3);
}

TEST_F(UnorderedMapTest, test_swap) {
  pc2l::Map<std::array<char, 10>, int> map;
  std::array<char, 10> appleArr = {'a', 'p', 'p', 'l', 'e'};
  map[appleArr] = 1;
  std::array<char, 10> bananaArr = {'b', 'a', 'n', 'a', 'n', 'a'};
  map[bananaArr] = 2;
  std::swap(map[appleArr], map[bananaArr]);
  ASSERT_EQ(map[appleArr], 2);
  ASSERT_EQ(map[bananaArr], 1);
}

TEST_F(UnorderedMapTest, test_transform) {
  pc2l::Map<std::array<char, 10>, int> map;
  std::array<char, 10> appleArr = {'a', 'p', 'p', 'l', 'e'};
  map[appleArr] = 1;
  std::array<char, 10> bananaArr = {'b', 'a', 'n', 'a', 'n', 'a'};
  map[bananaArr] = 2;
  std::transform(map.begin(), map.end(), map.begin(), [&](auto e) {
    return map.make_map_pair(e.first, e.second * 2);
  });
  ASSERT_EQ(map[appleArr], 2);
  ASSERT_EQ(map[bananaArr], 4);
}
