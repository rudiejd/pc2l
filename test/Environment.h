//
// Created by jd on 4/5/22.
//

#include "MPIHelper.h"
#ifndef PC2L_ENVIRONMENT_H
#include <gtest/gtest.h>
#include <mpi.h>
#include <pc2l.h>

pc2l::Vector<int, 8 * sizeof(int)> createRangeIntVec(int size) {
  pc2l::Vector<int, 8 * sizeof(int)> ret;
  for (int i = 0; i < size; i++) {
    ret.push_back(i);
  }
  return ret;
}

class PC2LEnvironment : public ::testing::Environment {
public:
  // define characteristics of the PC2L instance we will use for testing
  // we use small block sizes here just to test that features are working
  // correctly. Tests for performance are conducted in the examples and/or
  // benchmarks directory.
  const static unsigned int blockSize = sizeof(int) * 8;
  //    const static unsigned int cacheSize = 3 * (sizeof(pc2l::Message) +
  //    blockSize);

  ~PC2LEnvironment() override {};
  void SetUp() override {
    ::testing::TestEventListeners &listeners =
        ::testing::UnitTest::GetInstance()->listeners();
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank != 0) {
      delete listeners.Release(listeners.default_result_printer());
    }
  }
  void TearDown() override {}
};
#define PC2L_ENVIRONMENT_H

#endif // PC2L_ENVIRONMENT_H
