#include <benchmark/benchmark.h>
#include <ctime>

extern int *our_select(int arr[], size_t len, int rank);
extern int *rand_select(int arr[], size_t len, int rank);

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
    // generate a random array with nums in range [-len:len]
    int nread = 0;
    while(nread < m_len)
      nread = fread(&m_rnd_arr[nread], sizeof(*m_rnd_arr), m_len - nread, m_rnd_fd);
    for(size_t i = 0; i < m_len; ++i)
      m_rnd_arr[i] = abs(m_rnd_arr[i]) % (m_len * 2) - m_len;
    return m_rnd_arr;
  }
};

static void BM_select_impl(SelectFixture *fixture, benchmark::State& state, bool use_rand_select)
{
  int len = fixture->len();
  for (auto _ : state) {
    state.PauseTiming();
    int *rnd_arr = fixture->generate_random_array();
    state.ResumeTiming();

    int *res = use_rand_select
      ? rand_select(rnd_arr, len, 1 + (len / 2))
      : our_select(rnd_arr, len, 1 + (len / 2));
    if(res == NULL) {
      state.SkipWithError("Result is null, probably an allocation failure");
      break;
    }

    benchmark::DoNotOptimize(*res);
  }

  state.SetComplexityN(len);
}

BENCHMARK_DEFINE_F(SelectFixture, BM_select)(benchmark::State& state) { BM_select_impl(this, state, false); }
BENCHMARK_REGISTER_F(SelectFixture, BM_select)
  ->RangeMultiplier(2)
  ->Range(1 << 2, 1 << 18)
  ->Complexity();


BENCHMARK_DEFINE_F(SelectFixture, BM_rand_select)(benchmark::State& state) { BM_select_impl(this, state, true); }
BENCHMARK_REGISTER_F(SelectFixture, BM_rand_select)
  ->RangeMultiplier(2)
  ->Range(1 << 2, 1 << 18)
  ->Complexity();

BENCHMARK_MAIN();
