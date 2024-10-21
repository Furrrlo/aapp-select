#include <stdlib.h>
#include <stddef.h>

// Implementation of the method for searching the i-th smallest value of an array 
// of len elements, that recursively generate a good pivot 
int *our_select(int arr[], size_t len, int rank);

// Implementation of the RAND_SELECT method for the research of the i-th smallest value
// of an array of len elements, that recursively randomly select a pivot 
int *rand_select(int arr[], size_t len, int rank);

// Used when we want to calculate the median of any amount of elements
static int *median_of(int arr[], size_t len);

// Used when we want to calculate the median of exactly 5 elements
static int *median_of_5(int arr[]);

// Implementation of the PARTITION method, used for partitioning the 
// array of len elements into two subarrays around a pivot x such that 
// elements in lower subarray <= x <= elements in upper subarray
static size_t partition(int arr[], size_t len, int *pivot);

// Swap the value of two elements
static void swap(int *a1, int *a2);

// Sort two elements based of their value, putting the smaller in 
// a1 and the larger in a2
static void sort_min_max(int *a1, int *a2);

// Swap the value of two elements only if the condition is true
#define SWAP_IF(v1, v2, cond, a1, a2) \
  {                                   \
    int v1 = *a1, v2 = *a2;           \
    *a2 = cond ? v1 : v2;             \
    *a1 = cond ? v2 : v1;             \
  }

// Compare the values of two elements and returns a positive value 
// if a1 is higher than a2, a negative value if a1 is lower than a2, 
// otherwise return 0
int cmp_nums(const void *a1, const void *a2)
{
  // Do not use subtraction as it would overflow
  return (*(int*)a1 > *(int*)a2) ? 1 : (*(int*)a1 < *(int*)a2) ? -1 : 0;
}

int *our_select(int arr[], size_t len, int rank)
{
  // Checks for inconsistency of rank
  if(rank <= 0 || rank > len)
    return NULL;

  // When doing selection on a small array, we can consider it
  // constant time and just use a sorting function to find it
  if(len < 5)
  {
    qsort(arr, len, sizeof(*arr), cmp_nums);
    return &arr[rank - 1];
  }

  // Step 1. Divide the n elements into groups of 5 by selection. Find the median of each 5-element group by rote
  int len_mul_5 = len - (len % 5);
  // We need an array of len (len / 5) positions to store the medians
  // Instead of allocating it, let's just move stuff to the start of the arr we already have
  // This is the same thing that Wikipedia also does in its pseudocode of the method:
  // 'move the medians of five-element subgroups to the first n/5 positions'
  // https://en.wikipedia.org/wiki/Median_of_medians#Algorithm
  int *medians = arr;

  for(size_t i = 0; i < len_mul_5; i += 5)
  {
    // This never fails so we can skip error handling
    int *median_ptr = median_of_5(&arr[i]);
    // Put it in the medians array, preserving the substituted element
    swap(&medians[i], median_ptr);
  }
  // Step 2. Recursively SELECT the median x of the floor(n/5) group medians to be the pivot
  int *medians_of_medians = median_of(medians, len_mul_5 / 5); 
  if(!medians_of_medians)
    return NULL;

  // Step 3. Partition around the pivot. Let k = rank(pivot)
  size_t pivot_idx = partition(arr, len, medians_of_medians);
  size_t k = 1 + pivot_idx;
  // Step 4. Compare the rank of the pivot we've found with the i-th element we're searching
  // We've found exactly the i-th value, return this value
  if(rank == k)
    return &arr[pivot_idx];

  // Recursively SELECT the i-th smallest element in the lower part
  if(rank < k)
    return our_select(arr, pivot_idx, rank);
  
  // Recursively SELECT the (i–k)-th smallest element in the upper part
  return our_select(
      &arr[pivot_idx + 1], 
      len - pivot_idx - 1, 
      rank - k
  );
}

inline int *median_of_5(int a[])
{
  // https://www.ocf.berkeley.edu/%7Ewwu/cgi-bin/yabb/YaBB.cgi?board=riddles_cs;action=display;num=1061827085
  // https://www.ocf.berkeley.edu/~wwu/YaBBAttachments/median_of_five.gif
  // https://stackoverflow.com/a/481029
  // We do it branchless and hope it gets compiled with SIMD instructions
  sort_min_max(&a[0], &a[1]);
  sort_min_max(&a[3], &a[4]);
  sort_min_max(&a[0], &a[3]);
  sort_min_max(&a[1], &a[2]);
  sort_min_max(&a[1], &a[3]);
  sort_min_max(&a[2], &a[3]);
  return &a[2];
}

inline int *median_of(int arr[], size_t const len)
{
  return our_select(arr, len, 1 + len / 2);
}

int *rand_select(int arr[], size_t len, int rank)
{
  // Checks for inconsistency of rank
  if(rank <= 0 || rank > len)
    return NULL;
  
  // Randomly select a pivot among the element of the array
  int *rand_pivot = &arr[rand() % len];

  // Partitioning of the array based on the random pivot 
  size_t pivot_idx = partition(arr, len, rand_pivot);
  size_t k = 1 + pivot_idx;

  // We've found exactly the i-th value, return this value
  if(rank == k) 
    return &arr[pivot_idx];

  // recursively SELECT the i-th smallest element in the lower part
  if(rank < k)
    return rand_select(arr, pivot_idx, rank);
    
  // recursively SELECT the (i–k)-th smallest element in the upper part
  return rand_select(
      &arr[pivot_idx + 1], 
      len - pivot_idx - 1, 
      rank - k
  );
}

size_t partition(int arr[], size_t len, int *pivot)
{
  // Move the pivot on the first position of the array
  int p = *pivot;
  swap(arr, pivot);

  // Spit the array in two subarrays, a first part with all the elements 
  // <= than the pivot and a second part with all the elements
  // >= than the pivot
  size_t i = 0;
  for(size_t j = 1; j < len; ++j)
  {
    // Make it branchless and hope it gets compiled using magic
    i += arr[j] <= p;
    SWAP_IF(a, b, a <= p, &arr[j], &arr[i]);
  }

  // put the pivot in the middle of the two subarrays
  swap(arr, &arr[i]);
  return i;
}

inline void swap(int *a1, int *a2)
{
  int tmp = *a1;
  *a1 = *a2;
  *a2 = tmp;
}

static inline void sort_min_max(int *a1, int *a2)
{
  int a = *a1, b = *a2;
  *a1 = a < b ? a : b;
  *a2 = a < b ? b : a;
}
