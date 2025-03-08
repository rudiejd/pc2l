#include "Benchmark.h"
#include <algorithm>
#include <benchmark/benchmark.h>
#include <numeric>
#include <pc2l.h>

// Benchmark pc2l::Vector.at(i) when block containing index i is currently on
// the manager cache
static void
BM_at (benchmark::State &state)
{
  pc2l::Vector<int, 8 * sizeof (int)> v;
  for (int i = 0; i < 100; i++)
    {
      v.insert (i, i);
    }
  // so now the last 3 blocks will be in cache (default LRU strategy)
  // this is [94-99], [89-93], [84-88]
  // part that is timed is in this loop
  while (state.KeepRunning ())
    {
      v.at (state.range (0));
    }
}
BENCHMARK (BM_at)->Args ({ 0 });

static void
BM_at_prefetch (benchmark::State &state)
{
  pc2l::Vector<int, 8 * sizeof (int), 5, pc2l::FORWARD_SEQUENTIAL> v;
  for (int i = 0; i < 100; i++)
    {
      v.insert (i, i);
    }
  // so now the last 3 blocks will be in cache (default LRU strategy)
  // this is [94-99], [89-93], [84-88]
  // part that is timed is in this loop
  while (state.KeepRunning ())
    {
      v.at (state.range (0));
    }
}
BENCHMARK (BM_at_prefetch)->Args ({ 0 });

static void
BM_insert (benchmark::State &state)
{
  pc2l::Vector<int, 8 * sizeof (int)> v;
  // so now the last 3 blocks will be in cache (default LRU strategy)
  // this is [94-99], [89-93], [84-88]
  // part that is timed is in this loop
  while (state.KeepRunning ())
    {
      v.push_back (state.range (0));
    }
}
BENCHMARK (BM_insert)->Args ({ 0 });

static void
BM_replace (benchmark::State &state)
{
  pc2l::Vector<int, 8 * sizeof (int)> v;
  v.push_back (0);
  while (state.KeepRunning ())
    {
      v.replace (0, 1);
    }
}
BENCHMARK (BM_replace);

static void
BM_replace_out_of_cache (benchmark::State &state)
{
  pc2l::Vector<int, 8 * sizeof (int)> v;
  for (size_t i = 0; i < 100; i++)
    {
      v.push_back (i);
    }

  // the first block should be out of cache
  while (state.KeepRunning ())
    {
      v.replace (0, 1);
    }
}
BENCHMARK (BM_replace_out_of_cache);

static void
BM_insert_at_beginning (benchmark::State &state)
{
  pc2l::Vector<int, 8 * sizeof (int)> v;
  for (int i = 0; i < 100; i++)
    {
      v.push_back (i);
    }
  while (state.KeepRunning ())
    {
      v.insert (0, state.range (0));
    }
}
BENCHMARK (BM_insert_at_beginning)->Args ({ 0 });

static void
BM_find_out_of_cache (benchmark::State &state)
{
  pc2l::Vector<int, 8 * sizeof (int)> v;
  for (int i = 0; i < state.range (0); i++)
    {
      v.push_back (i);
    }
  while (state.KeepRunning ())
    {
      std::find (v.begin (), v.end (), state.range (0) / 2);
    }
}
BENCHMARK (BM_find_out_of_cache)
    ->RangeMultiplier (10)
    ->Range (10, 10000000000);

static void
BM_project_euler (benchmark::State &state)
{
  pc2l::Vector<unsigned long long> vec (state.range (0));

  while (state.KeepRunning ())
    {
      // fill vector with values from 1 to n
      std::iota (vec.begin (), vec.end (), 1);

      // every number that does not have a 3 or 5 as a factor is set to 0
      std::replace_if (
          vec.begin (), vec.end (),
          [] (auto i) { return !((i % 3) || (i % 5)); }, 0);

      // sum all elements in the vector
      auto total = std::accumulate (vec.begin (), vec.end (), 0ULL);
    }
}
BENCHMARK (BM_project_euler)->RangeMultiplier (10)->Range (10, 10000000000);

static void
BM_std_sort (benchmark::State &state)
{
  pc2l::Vector<unsigned long long> vec;
  for (auto i = 0; i < state.range (0); i++)
    {
      vec.push_back (i);
    }
  while (state.KeepRunning ())
    {
      std::sort (vec.begin (), vec.end ());
    }
}

BENCHMARK (BM_std_sort)->RangeMultiplier (10)->Range (10, 10000000000);
static void
BM_find_in_cache (benchmark::State &state)
{
  pc2l::Vector<int, 8 * sizeof (int)> v;
  for (int i = 0; i < 100; i++)
    {
      v.push_back (i);
    }
  while (state.KeepRunning ())
    {
      std::find (v.begin (), v.end (), 99);
    }
}
BENCHMARK (BM_find_in_cache);

int
main (int argc, char **argv)
{
  auto &pc2l = pc2l::System::get ();
  // Override the default block size to 8 ints
  const int BlockSize = sizeof (int) * 8;
  // set cache size to 3 blocks
  pc2l.setCacheSize (3 * (BlockSize + sizeof (pc2l::Message)));
  pc2l.initialize (argc, argv);
  pc2l.start ();

  benchmark::Initialize (&argc, argv);
  if (pc2l::MPI_GET_RANK () == 0)
    {
      benchmark::RunSpecifiedBenchmarks ();
    }
  else
    {
      NullReporter null;
      benchmark::RunSpecifiedBenchmarks (&null);
    }
  benchmark::Shutdown ();

  pc2l.stop ();
  pc2l.finalize ();
}
