#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctime>
#include "test.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

extern int *our_select(int arr[], size_t len, int rank);
extern int *rand_select(int arr[], size_t len, int rank);
extern int cmp_nums(const void *a1, const void *a2);

TEST_INIT_RUNNER((int const *initial, int *a, int len, int rk, char const *test_name) { 
  // Run using our function and check for potential failures
  memcpy(a, initial, len * sizeof(*initial));
  int *actual_ptr = our_select(a, len, rk);
  if(!actual_ptr) {
    printf(ANSI_COLOR_RED "FAIL" ANSI_COLOR_RESET " '%s'\n"
        "    Failed to run our_select, could be an allocation failure\n",
        test_name);
    return;
  }
  int actual = *actual_ptr;

  // Run using rand_select and check for potential failures
  memcpy(a, initial, len * sizeof(*initial));
  int *actual_rand_ptr = rand_select(a, len, rk);
  if(!actual_rand_ptr) {
    printf(ANSI_COLOR_RED "FAIL" ANSI_COLOR_RESET " '%s'\n"
        "    Failed to run rand_select, could be an allocation failure\n",
        test_name);
    return;
  }
  int actual_rand = *actual_rand_ptr;

  // Run using qsort and extracting the rk-th element from the sorted array
  memcpy(a, initial, len * sizeof(*initial));
  qsort(a, len, sizeof(*a), cmp_nums);
  int expected = a[rk];

  // Compare the two results and print whether they match or not
  printf("%s" ANSI_COLOR_RESET " '%s'\n"
         "    expected %d, got deterministic %d and rand %d",
         expected == actual && expected == actual_rand ? ANSI_COLOR_GREEN "PASS" : ANSI_COLOR_RED "FAIL",
         test_name, expected, actual, actual_rand);
  
  if(len < 30) {
    printf(", sorted arr: {");
    for(size_t i = 0; i < len; ++i)
      printf("%d, ", a[i]);
    printf("}");
  }
  
  printf("\n");
})

TEST_CASE(TEST_ARR({ 3, 84, 2, 9, 12, 50 }), 4, "(len % 5 == 1)")
TEST_CASE(TEST_ARR({ 3, 7, 84, 12, 15, 17, 23, 11, 98, 81, 60, 50 }), 5, "(len % 5 == 2)")
TEST_CASE(TEST_ARR({ 3, 7, 84, 2, 9, 12, 15, 23, 11, 98, 81, 60, 50 }), 7, "(len % 5 == 3)")
TEST_CASE(TEST_ARR({ 3, 7, 84, 2, 9, 12, 15, 17, 23, 11, 98, 81, 60, 50 }), 3, "(len % 5 == 4)")
TEST_CASE(TEST_ARR({ 3, 7, 84, 2, 9, 12, 15, 17, 23, 11, 98, 81, 60, 50, 40, 6 }), 3, "ends up in the lower part of recursion")
TEST_CASE(TEST_ARR({ 3, 7, 84, 2, 9, 12, 15, 17, 23, 11, 98, 81, 60, 50, 40, 6 }), 12, "ends up in the higher part of recursion")

// Random testing with huge numbers
TEST_CASE_ON_STACK({
  FILE *rnd_fd = fopen("/dev/urandom", "r");
  if(!rnd_fd)
  {
    printf("Failed to open /dev/urandom");
    return;
  }

  int rnd_arr[100000];
  int nread = 0;
  while(nread < sizeof(rnd_arr) / sizeof(*rnd_arr))
    nread = fread(&rnd_arr[nread], sizeof(*rnd_arr), (sizeof(rnd_arr) / sizeof(*rnd_arr)) - nread, rnd_fd);
  
  TEST_CASE_BODY(rnd_arr, 500,  "random 1");
  TEST_CASE_BODY(rnd_arr, 214,  "random 2");
  TEST_CASE_BODY(rnd_arr, 8,    "random 3");
  TEST_CASE_BODY(rnd_arr, 58,   "random 4");
 
  fclose(rnd_fd);
})

TEST_MAIN({
    // seed the rand() function for use within rand_select()
    srand(time(NULL));
})
