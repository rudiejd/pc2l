#include <benchmark/benchmark.h>

class PC2LFixture : public benchmark::Fixture {
public:
  PC2LFixture() {}
};

// Benchmark pc2l::Vector.at(i) when block containing index i is currently on
// the manager cache
static void BM_at(benchmark::State &state) {
  std::vector<int> v;
  for (int i = 0; i < state.range(0); i++) {
    v.push_back(i);
  }

  while (state.KeepRunning()) {
    v.at(state.range(0) / 2);
  }
}
BENCHMARK(BM_at)->RangeMultiplier(10)->Range(10, 10000000000);

static void BM_insert(benchmark::State &state) {
  std::vector<int> v;
  for (int i = 0; i < state.range(0); i++) {
    v.push_back(i);
  }

  while (state.KeepRunning()) {
    v.push_back(1);
  }
}
BENCHMARK(BM_insert)->RangeMultiplier(10)->Range(10, 10000000000);

static void BM_insert_at_beginning(benchmark::State &state) {
  std::vector<int> v;
  for (int i = 0; i < state.range(0); i++) {
    v.push_back(i);
  }
  while (state.KeepRunning()) {
    v.insert(v.begin(), 1);
  }
}
BENCHMARK(BM_insert)->RangeMultiplier(10)->Range(10, 10000000000);

static void BM_find_middle(benchmark::State &state) {
  std::vector<int> v;
  auto size = state.range(0);
  for (int i = 0; i < size; i++) {
    v.push_back(i);
  }
  while (state.KeepRunning()) {
    std::find(v.begin(), v.end(), size / 2);
  }
}
BENCHMARK(BM_find_middle)->RangeMultiplier(10)->Range(10, 10000000000);

int main(int argc, char **argv) {
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
}
