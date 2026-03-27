#include "sort.hxx"

#include <benchmark/benchmark.h>

#include <numeric>
#include <random>

static void BM_SortA (benchmark::State& state) {
  std::vector<int> data(state.range(0));
  for ( auto _: state ) {
    state.PauseTiming( );
    std::iota(data.begin( ), data.end( ), 0);
    std::ranges::shuffle(data, std::mt19937(std::random_device( )( )));
    state.ResumeTiming( );
    wbr::sort_a(data);
    benchmark::DoNotOptimize(data);
  }
  state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_SortA)->RangeMultiplier(2)->Range(1 << 4, 1 << 16)->Complexity(benchmark::oNLogN);
