#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h> 
#include <sys/time.h>
#include "../lib/utils.c"

#define PAD 8 // Assume 64 byte L1 cache line size with 8 double elements

typedef int thread_id_t;

/**
 * @brief Calculates the approximation of pi using the Leibniz series
 *        going from n to N where n is `from` and the N is `to`
 * 
 * @param from Initial number of the series
 * @param to Number of terms at the end of the series
 */
static double pi_approx(long from, long to)
{
  double pi = 0.0;
  int sign = 0;

  if (from % 2 == 0) sign = 1;
  else sign = -1;

  for (long i = from; i <= to; i++)
  {
    pi += sign / (2.0 * (double) i + 1.0);
    sign *= -1;
  }

  return pi;
}

static double ** parallel_pi_approx_ts(const int n_threads, const long terms, double pi[n_threads][PAD])
{
  #pragma omp parallel num_threads(n_threads)
  {
    thread_id_t thread_id = omp_get_thread_num();

    long chunk = terms / n_threads;
    long from = thread_id * terms;
    long to = from + chunk - 1;

    double current_pi = pi_approx(from, to);


    pi[thread_id][0] = current_pi; 
  }

  return pi;
}

int main(int argc, char const *argv[])
{
  const int n_threads = atoi(argv[1]);
  const long terms    = 2e9;
  double pi_mtx[n_threads][PAD];
  double pi_arr[n_threads];

  parallel_pi_approx_ts(n_threads, terms, pi_mtx);

  for(int i = 0; i < n_threads; i++)
  {
    pi_arr[i] = pi_mtx[i][0];
  }



  const double pi = 4 * sum_arr(n_threads, pi_arr);

  printf("Value of π with %2d threads is: %.16f, error: %0.6e\n", n_threads, pi, fabs(M_PI - pi));

  return EXIT_SUCCESS;
}