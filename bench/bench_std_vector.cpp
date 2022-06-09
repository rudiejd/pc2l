#include <benchmark/benchmark.h>

class PC2LFixture : public benchmark::Fixture {
public:
    PC2LFixture() {
    }
};

// Benchmark pc2l::Vector.at(i) when block containing index i is currently on the manager cache
static void BM_at(benchmark::State& state) {
    std::vector<int> v;
    for (int i = 0; i < 100; i++) {
        v.push_back(i);
    }

    while (state.KeepRunning()) {
        v.at(state.range(0));
    }
}

BENCHMARK(BM_at)
    ->Args({99})
    ->Args({98})
    ->Args({97})
    ->Args({96})
    ->Args({95})
    ->Args({94})
    ->Args({93})
    ->Args({92})
    ->Args({91})
    ->Args({90});

int main(int argc, char** argv) {
    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
}
