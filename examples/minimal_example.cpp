#ifndef MINIMAL_EXAMPLE_CPP
#define MINIMAL_EXAMPLE_CPP

#include "MPIHelper.h"
#include <pc2l.h>

int main(int argc, char *argv[]) {
  // Boilerplate code to get MPI up and running
  auto &pc2l = pc2l::System::get();
  pc2l.initialize(argc, argv);
  // 50 block cache
  pc2l.setCacheSize(50 * 100 * sizeof(int));
  pc2l.start();

  if (pc2l::MPI_GET_RANK() == 0) {

    pc2l::Vector<int, 8 * sizeof(int)> vec;

    for (auto i = 0; i < 100; i++) {
      vec.push_back(i);
    }

    for (auto i = 0; i < vec.size(); i++) {
      std::cout << "vec[" << i << "]=" << vec[i] << std::endl;
    }
  }

  pc2l.stop();
  pc2l.finalize();
}

#endif
