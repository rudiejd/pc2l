#include "Benchmark.h"
#include <algorithm>
#include <benchmark/benchmark.h>
#include <numeric>
#include <pc2l.h>

// 100 MB blocks
const auto BLOCK_SIZE = 100 * 1024 * 1024;

// Benchmark pc2l::Vector.at(i) when block containing index i is currently on
// the manager cache
static void BM_at(benchmark::State &state) {
  pc2l::Vector<int, BLOCK_SIZE> v;
  for (int i = 0; i < state.range(0); i++) {
    v.insert(i, i);
  }
  // so now the last 3 blocks will be in cache (default LRU strategy)
  // this is [94-99], [89-93], [84-88]
  // part that is timed is in this loop
  while (state.KeepRunning()) {
    v.at(state.range(0));
  }
}
BENCHMARK(BM_at)->RangeMultiplier(10)->Range(10, 10000000000);

static void BM_insert(benchmark::State &state) {
  pc2l::Vector<int, BLOCK_SIZE> v;
  // so now the last 3 blocks will be in cache (default LRU strategy)
  // this is [94-99], [89-93], [84-88]
  // part that is timed is in this loop
  while (state.KeepRunning()) {
    v.push_back(1);
  }
}
BENCHMARK(BM_insert)->RangeMultiplier(10)->Range(10, 10000000000);

static void BM_insert_at_beginning(benchmark::State &state) {
  pc2l::Vector<int, BLOCK_SIZE> v;
  for (int i = 0; i < state.range(0); i++) {
    v.push_back(i);
  }
  while (state.KeepRunning()) {
    v.insert(v.begin(), 1);
  }
}
BENCHMARK(BM_insert)->RangeMultiplier(10)->Range(10, 10000000000);

static void BM_find_middle(benchmark::State &state) {
  auto size = state.range(0);

  // 100 megabyte blocks
  pc2l::Vector<int, 100 * 1024 * 1024> v;
  for (int i = 0; i < size; i++) {
    v.push_back(i);
  }
  while (state.KeepRunning()) {
    std::find(v.begin(), v.end(), size / 2);
  }
}
BENCHMARK(BM_find_middle)->RangeMultiplier(10)->Range(10, 10000000000);

static void BM_std_sort(benchmark::State &state) {
  pc2l::Vector<unsigned long long> vec;
  for (auto i = 0; i < state.range(0); i++) {
    vec.push_back(i);
  }
  while (state.KeepRunning()) {
    std::sort(vec.begin(), vec.end());
  }
}

BENCHMARK(BM_std_sort)->RangeMultiplier(10)->Range(10, 10000000000);
static void BM_find_in_cache(benchmark::State &state) {
  pc2l::Vector<int, BLOCK_SIZE> v;
  for (int i = 0; i < 100; i++) {
    v.push_back(i);
  }
  while (state.KeepRunning()) {
    std::find(v.begin(), v.end(), 99);
  }
}
BENCHMARK(BM_find_in_cache);

int main(int argc, char **argv) {
  auto &pc2l = pc2l::System::get();
  // set cache size to 20 mb
  pc2l.setCacheSize(BLOCK_SIZE * 20);
  pc2l.initialize(argc, argv);
  pc2l.start();

  benchmark::Initialize(&argc, argv);
  if (pc2l::MPI_GET_RANK() == 0) {
    benchmark::RunSpecifiedBenchmarks();
  } else {
    NullReporter null;
    benchmark::RunSpecifiedBenchmarks(&null);
  }
  benchmark::Shutdown();

  pc2l.stop();
  pc2l.finalize();
}
