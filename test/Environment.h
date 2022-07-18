//
// Created by jd on 4/5/22.
//

#ifndef PC2L_ENVIRONMENT_H
#include <gtest/gtest.h>
#include <pc2l.h>
#include <mpi.h>

class PC2LEnvironment : public ::testing::Environment {
public:
    int argc;
    char** argv;
    // define characteristics of the PC2L instance we will use for testing
    // we use small block sizes here just to test that features are working
    // correctly. Tests for performance are conducted in the examples and/or
    // benchmarks directory.
    const static unsigned int blockSize = sizeof(int) * 8;
//    const static unsigned int cacheSize = 3 * (sizeof(pc2l::Message) + blockSize);

    ~PC2LEnvironment() override {};
    void SetUp() override {
        auto& pc2l = pc2l::System::get();
        pc2l.setBlockSize(blockSize);
//        pc2l.setCacheSize(cacheSize);
        pc2l.initialize(argc, argv);
        pc2l.start();
        ::testing::TestEventListeners& listeners =
                ::testing::UnitTest::GetInstance()->listeners();
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        if (rank != 0) {
            delete listeners.Release(listeners.default_result_printer());
        }
    }
    void TearDown() override {
        auto& pc2l = pc2l::System::get();
        pc2l.stop();
        pc2l.finalize();
    }
};
#define PC2L_ENVIRONMENT_H

#endif //PC2L_ENVIRONMENT_H
