#include <benchmark/benchmark.h>

class PC2LFixture : public benchmark::Fixture
{
public:
  PC2LFixture () {}
};

// Benchmark pc2l::Vector.at(i) when block containing index i is currently on
// the manager cache
static void
BM_at (benchmark::State &state)
{
  std::vector<int> v;
  for (int i = 0; i < 100; i++)
    {
      v.push_back (i);
    }

  while (state.KeepRunning ())
    {
      v.at (state.range (0));
    }
}

BENCHMARK (BM_at)
    ->Args ({ 99 })
    ->Args ({ 98 })
    ->Args ({ 97 })
    ->Args ({ 96 })
    ->Args ({ 95 })
    ->Args ({ 94 })
    ->Args ({ 93 })
    ->Args ({ 92 })
    ->Args ({ 91 })
    ->Args ({ 90 });

static void
BM_insert (benchmark::State &state)
{
  std::vector<int> v;
  // so now the last 3 blocks will be in cache (default LRU strategy)
  // this is [94-99], [89-93], [84-88]
  // part that is timed is in this loop
  while (state.KeepRunning ())
    {
      v.push_back (state.range (0));
    }
}

BENCHMARK (BM_insert)->DenseRange (0, 10);

static void
BM_insert_at_beginning (benchmark::State &state)
{
  std::vector<int> v;
  for (int i = 0; i < 100; i++)
    {
      v.push_back (i);
    }
  while (state.KeepRunning ())
    {
      v.insert (v.begin (), state.range (0));
    }
}

BENCHMARK (BM_insert_at_beginning)->DenseRange (0, 10);

static void
BM_find_middle (benchmark::State &state)
{
  std::vector<int> v;
  for (int i = 0; i < 100; i++)
    {
      v.push_back (i);
    }
  while (state.KeepRunning ())
    {
      std::find (v.begin (), v.end (), 50);
    }
}
BENCHMARK (BM_find_middle);

int
main (int argc, char **argv)
{
  benchmark::Initialize (&argc, argv);
  benchmark::RunSpecifiedBenchmarks ();
  benchmark::Shutdown ();
}
