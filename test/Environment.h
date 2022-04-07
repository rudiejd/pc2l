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
    ~PC2LEnvironment() override {};
    void SetUp() override {
        // set block size to 5 integers
        auto& pc2l = pc2l::System::get();
        pc2l.setBlockSize(sizeof(int) * 5);
        // set cache size to 3 blocks
        pc2l.setCacheSize(3*5*sizeof(int));
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
