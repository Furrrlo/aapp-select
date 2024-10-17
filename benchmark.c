#include <benchmark/benchmark.h>
#include <ctime>

extern int *our_select(int arr[], size_t len, int rank);
extern int *rand_select(int arr[], size_t len, int rank);

static void BM_select_impl(benchmark::State& state, bool use_rand_select)
{
  // open the random fd
  FILE *rnd_fd = fopen("/dev/urandom", "r");
  if(!rnd_fd)
  {
    state.SkipWithError("Failed to open /dev/urandom");
    return;
  }

  // Allocate an array of len given by the benchmark lib
  int len = state.range(0);
  int *rnd_arr = (int*) malloc(len * sizeof(*rnd_arr));
  if(!rnd_arr)
  {
    state.SkipWithError("Failed to allocate random array");
    return;
  }
 
  // Run the benchmark
  for (auto _ : state) {
    state.PauseTiming();
    
    // generate a random array 
    int nread = 0;
    while(nread < len)
      nread = fread(&rnd_arr[nread], sizeof(*rnd_arr), len - nread, rnd_fd);

    // pick a random number which is inside the [0:len) range
    int rnd_pick;
    while(!fread(&rnd_pick, sizeof(rnd_pick), 1, rnd_fd));
    rnd_pick = abs(rnd_pick) % len;

    state.ResumeTiming();

    int *res = use_rand_select
      ? rand_select(rnd_arr, len, rnd_pick)
      : our_select(rnd_arr, len, rnd_pick);
    if(res == NULL) {
      state.SkipWithError("Result is null, probably an allocation failure");
      break;
    }

    benchmark::DoNotOptimize(*res);
  }

  state.SetComplexityN(len);
  free(rnd_arr);
  fclose(rnd_fd);
}

static void BM_select(benchmark::State& state)
{
  BM_select_impl(state, false);
}

static void BM_rand_select(benchmark::State& state)
{
  // seed the rand() function for use within rand_select()
  srand(time(NULL));
  BM_select_impl(state, true);
}

BENCHMARK(BM_select)
  ->RangeMultiplier(2)
  ->Range(1 << 10, 1 << 20)
  ->Complexity();

BENCHMARK(BM_rand_select)
  ->RangeMultiplier(2)
  ->Range(1 << 10, 1 << 20)
  ->Complexity();

BENCHMARK_MAIN();
