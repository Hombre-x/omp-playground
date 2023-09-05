#include <time.h>
#include <omp.h>

/**
 * @brief Sums all the elements inside an array `arr` of type double
 * 
 * @param arr_lenght The lenght of the array `arr`
 * @param arr Pointer to the array `arr`
 * @return The sum of the contents of the array.
 */
static double sum_arr(const long arr_lenght, double * arr)
{
  double sum = 0.0;

  for (long i = 0; i < arr_lenght; i++)
  {
    sum += arr[i];
  }

  return sum;
}


static double measure_time(void * func(void *), void * arg, void * result)
{
  double start_time = 0L, end_time = 0L;
  double elapsed_time = 0.0;

  start_time = omp_get_wtime();

  result = func(arg);

  end_time = omp_get_wtime();
  
  elapsed_time = ((double) (end_time - start_time));


  return elapsed_time;
}
