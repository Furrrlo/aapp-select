#include <stdlib.h>
#include <stddef.h>

int *our_select(int arr[], size_t len, int rank);
int *rand_select(int arr[], size_t len, int rank);
int *median_of(int arr[], size_t len);
int *median_of_5(int arr[]);
size_t partition(int arr[], size_t len, int *pivot);
void swap(int *a1, int *a2);

int cmp_nums(const void *a1, const void *a2)
{
  // Do not use subtraction as it would overflow
  return (*(int*)a1 > *(int*)a2) ? 1 : (*(int*)a1 < *(int*)a2) ? -1 : 0;
}

int *our_select(int arr[], size_t len, int rank)
{
  if(rank <= 0 || rank > len)
    return NULL;

  // When doing a median on a small array, we can consider it
  // constant time and just use a sorting function to find it
  if(len <= 5)
  {
    qsort(arr, len, sizeof(*arr), cmp_nums);
    return &arr[rank - 1];
  }

  // Step 1
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
  // Step 2
  int *medians_of_medians = median_of(medians, len_mul_5 / 5); 
  if(!medians_of_medians)
    return NULL;

  // Step 3
  size_t pivot_idx = partition(arr, len, medians_of_medians);
  size_t k = 1 + pivot_idx;
  // Step 4
  // i = rank
  if(rank == k)
    return &arr[pivot_idx];

  if(rank < k)
    return our_select(arr, pivot_idx, rank);
   
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
  if(a[0] >= a[1]) swap(&a[0], &a[1]);
  if(a[3] >= a[4]) swap(&a[3], &a[4]);
  if(a[0] >= a[3]) swap(&a[0], &a[3]);
  if(a[2] > a[1])
  {
    if(a[1] < a[3])
      return a[2] < a[3] ? &a[2] : &a[3];
    return a[1] < a[4] ? &a[1] : &a[4];
  }
  if(a[2] > a[3])
    return a[2] < a[4] ? &a[2] : &a[4];
  return a[1] < a[3] ? &a[1] : &a[3];
}

inline int *median_of(int arr[], size_t const len)
{
  return our_select(arr, len, 1 + len / 2);
}

int *rand_select(int arr[], size_t len, int rank)
{
  if(rank <= 0 || rank > len)
    return NULL;
  
  int *rand_pivot = &arr[rand() % len];
  size_t pivot_idx = partition(arr, len, rand_pivot);
  size_t k = 1 + pivot_idx;
  // i = rank
  if(rank == k) 
    return &arr[pivot_idx];

  if(rank < k)
    return our_select(arr, pivot_idx, rank);
    
  return our_select(
      &arr[pivot_idx + 1], 
      len - pivot_idx - 1, 
      rank - k
  );
}

size_t partition(int arr[], size_t len, int *pivot)
{
  swap(arr, pivot);
  pivot = arr;

  size_t i = 0;
  for(size_t j = 1; j < len; j++)
  {
    if(arr[j] <= *pivot)
    {
      i++;
      swap(&arr[j], &arr[i]);
    }
  } 
  
  swap(arr, &arr[i]);
  return i;
}

inline void swap(int *a1, int *a2)
{
  int tmp = *a1;
  *a1 = *a2;
  *a2 = tmp;
}
