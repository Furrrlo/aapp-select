#include <benchmark/benchmark.h>
#include <bits/stdc++.h>
#include <ctime>

extern int *our_select(int arr[], size_t len, int rank);
extern int *rand_select(int arr[], size_t len, int rank);
extern int cmp_nums(const void *a1, const void *a2);

class SelectFixture : public benchmark::Fixture
{
private:
  FILE *m_rnd_fd;
  int *m_rnd_arr;
  int m_len;
public:
  void SetUp(::benchmark::State& state)
  {
    // seed the rand() function for use within rand_select()
    srand(time(NULL));

    // open the random fd
    m_rnd_fd = fopen("/dev/urandom", "r");
    if(!m_rnd_fd)
    {
      state.SkipWithError("Failed to open /dev/urandom");
      return;
    }

    // Allocate an array of len given by the benchmark lib
    m_len = state.range(0);
    m_rnd_arr = (int*) malloc(m_len * sizeof(*m_rnd_arr));
    if(!m_rnd_arr)
    {
      state.SkipWithError("Failed to allocate random array");
      return;
    }
  }

  void TearDown(::benchmark::State& state)
  {
    free(m_rnd_arr);
    fclose(m_rnd_fd);
  }

  int len() { return m_len; }
  int *generate_random_array()
  {
    // generate a random array
    // we avoid duplicates as our implementation has only an O(n) guarantee when there are none,
    // to also support O(n) with dupes we would need to use a three-way partition. See:
    // - https://en.wikipedia.org/wiki/Dutch_national_flag_problem
    // - https://en.wikipedia.org/wiki/Median_of_medians#Partition_helper_functions
    std::unordered_set<int> seen;
    seen.reserve(m_len);

    int nread = 0;
    while(nread < m_len)
      nread += fread(&m_rnd_arr[nread], sizeof(*m_rnd_arr), m_len - nread, m_rnd_fd);
    
    for(int i = 0; i < m_len; ++i)
    {
      // Regen until not duped
      while(seen.find(m_rnd_arr[i]) != seen.end()
          && fread(&m_rnd_arr[i], sizeof(*m_rnd_arr), 1, m_rnd_fd));
      seen.insert(m_rnd_arr[i]);
    }
    return m_rnd_arr;
  }
};

static void BM_select_impl(SelectFixture *fixture, benchmark::State& state, int fn)
{
  int len = fixture->len();
  for (auto _ : state) {
    // Pause timers to have a somewhat correct CPU time, even if they add a bit of overhead
    state.PauseTiming();
    int *rnd_arr = fixture->generate_random_array();
    state.ResumeTiming();

    // Use manual timing to have an accurate real time, without the Pause/Resume overhead
    auto start = std::chrono::high_resolution_clock::now();
    switch(fn) {
    case 0: benchmark::DoNotOptimize(our_select(rnd_arr, len, 1 + (len / 2))); break;
    case 1: benchmark::DoNotOptimize(rand_select(rnd_arr, len, 1 + (len / 2))); break;
    case 2:
      qsort(rnd_arr, len, sizeof(*rnd_arr), cmp_nums);
      benchmark::DoNotOptimize(rnd_arr[1 + (len / 2)]);
      break;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds =
      std::chrono::duration_cast<std::chrono::duration<double>>(
        end - start);

    state.SetIterationTime(elapsed_seconds.count());
  }

  state.SetComplexityN(len);
}

BENCHMARK_DEFINE_F(SelectFixture, BM_select)(benchmark::State& state) { BM_select_impl(this, state, 0); }
BENCHMARK_REGISTER_F(SelectFixture, BM_select)
  ->UseManualTime()
  ->RangeMultiplier(2)
  ->Range(1 << 2, 1 << 18)
  ->Complexity();

BENCHMARK_DEFINE_F(SelectFixture, BM_rand_select)(benchmark::State& state) { BM_select_impl(this, state, 1); }
BENCHMARK_REGISTER_F(SelectFixture, BM_rand_select)
  ->UseManualTime()
  ->RangeMultiplier(2)
  ->Range(1 << 2, 1 << 18)
  ->Complexity();

BENCHMARK_DEFINE_F(SelectFixture, BM_qsort)(benchmark::State& state) { BM_select_impl(this, state, 2); }
BENCHMARK_REGISTER_F(SelectFixture, BM_qsort)
  ->UseManualTime()
  ->RangeMultiplier(2)
  ->Range(1 << 2, 1 << 18)
  ->Complexity(benchmark::oNLogN);

BENCHMARK_MAIN();
