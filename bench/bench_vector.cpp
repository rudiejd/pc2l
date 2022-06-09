#include <benchmark/benchmark.h>
#include <pc2l.h>

class PC2LFixture : public benchmark::Fixture {
public:
    PC2LFixture() {
    }
};

// Benchmark pc2l::Vector.at(i) when block containing index i is currently on the manager cache
static void BM_at(benchmark::State& state) {
    pc2l::Vector<int> v;
    for (int i = 0; i < 100; i++) {
        v.insert(i, i);
    }
    // so now the last 3 blocks will be in cache (default LRU strategy)
    // this is [94-99], [89-93], [84-88]
    // part that is timed is in this loop
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

static void BM_insert(benchmark::State& state) {
    pc2l::Vector<int> v;
    // so now the last 3 blocks will be in cache (default LRU strategy)
    // this is [94-99], [89-93], [84-88]
    // part that is timed is in this loop
    while (state.KeepRunning()) {
        v.push_back(state.range(0));
    }
}

BENCHMARK(BM_insert)->DenseRange(0, 100);


int main(int argc, char** argv) {
    auto& pc2l = pc2l::System::get();
    // set block size to 5 ints
    pc2l.setBlockSize(sizeof(int) * 5);
    // set cache size to 3 blocks
    pc2l.setCacheSize(3*5*sizeof(int));
    pc2l.initialize(argc, argv);
    pc2l.start();

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
}
