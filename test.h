#define CONCAT(x, y) CONCAT2(x, y)
#define CONCAT2(x, y) x##y

#define TEST_INIT_RUNNER(...) \
  typedef void (*test_case_t)(); \
  test_case_t test_cases[100]; \
  int test_cases_idx = 0; \
  \
  int add_test_case(test_case_t test_case) \
  { \
    test_cases[test_cases_idx++] = test_case; \
    return 0; \
  } \
  \
  void __run_test __VA_ARGS__
  

#define TEST_ARR(...) __VA_ARGS__
#define TEST_CASE(arr, ...) TEST_CASE_ON_STACK({ \
    int initial_arr[] = arr; \
    TEST_CASE_BODY(initial_arr, __VA_ARGS__) \
  })

#define TEST_CASE_ON_STACK(...) \
  void CONCAT(test_method_, __LINE__)() { \
    __VA_ARGS__ \
  } \
  \
  int CONCAT(test_dummy_, __LINE__) = add_test_case(CONCAT(test_method_, __LINE__));

#define TEST_CASE_BODY(initial_arr, ...) { \
  int test_arr[sizeof(initial_arr) / sizeof(*initial_arr)]; \
  int len = sizeof(initial_arr) / sizeof(*initial_arr); \
  __run_test(initial_arr, test_arr, len, __VA_ARGS__); \
}

#define TEST_MAIN() \
  int main() \
  { \
    for(size_t i = 0; i < test_cases_idx; ++i) \
      test_cases[i](); \
    return 0; \
  }
