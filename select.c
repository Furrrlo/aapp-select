#include <stdlib.h>
#include <stddef.h>

int *our_select(int arr[], size_t len, int rank);
int *median_of(int arr[], size_t len);
size_t partition(int arr[], size_t len, int *pivot);
void swap(int *a1, int *a2);

int cmp_nums(const void *a1, const void *a2)
{
  // Do not use subtraction as it would overflow
  return (*(int*)a1 > *(int*)a2) ? 1 : (*(int*)a1 < *(int*)a2) ? -1 : 0;
}

int *our_select(int arr[], size_t len, int rank)
{
  if(rank >= len)
    return NULL;

  // When doing a median on a small array, we can consider it
  // constant time and just use a sorting function to find it
  if(len <= 5)
  {
    qsort(arr, len, sizeof(*arr), cmp_nums);
    return &arr[rank];
  }

  // Step 1
  int *medians;
  int **medians_ptrs;
  int len_mul_5 = len - (len % 5);

  medians = (int*) malloc((len_mul_5 / 5) * sizeof(*medians));
  if(!medians)
    return NULL;
  
  medians_ptrs = (int**) malloc((len_mul_5 / 5) * sizeof(*medians_ptrs));
  if(!medians_ptrs) {
    free(medians);
    return NULL;
  }

  for(size_t i = 0; i < len_mul_5; i += 5)
  {
    // For small arrays, this never fails so we can skip error handling
    int *median_ptr = median_of(&arr[i], 5);
    medians[i / 5] = *median_ptr;
    medians_ptrs[i / 5] = median_ptr;
  }
  // Step 2
  int *medians_of_medians = median_of(medians, len_mul_5 / 5); 
  if(!medians_of_medians)
  {
    free(medians);
    free(medians_ptrs);
    return NULL;
  }

  // Move the pointer from the 'medians' array back to the 'arr' array
  medians_of_medians = medians_ptrs[medians_of_medians - medians];
  free(medians);
  free(medians_ptrs);

  // Step 3
  size_t pivot_idx = partition(arr, len, medians_of_medians);
  // Step 4
  // k = pivot_idx = rank(x)
  // i = rank
  if(rank == pivot_idx) 
    return &arr[pivot_idx];

  if(rank < pivot_idx)
    return our_select(arr, pivot_idx, rank);
    
  return our_select(
      &arr[pivot_idx + 1], 
      len - pivot_idx - 1, 
      rank - pivot_idx - 1 /* the -1 was not on the slides */
  );
}

int *median_of(int arr[], size_t const len)
{
  return our_select(arr, len, len / 2);
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

void swap(int *a1, int *a2)
{
  int tmp = *a1;
  *a1 = *a2;
  *a2 = tmp;
}
