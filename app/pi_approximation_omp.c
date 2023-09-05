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
 * @brief Struct to pass arguments to omp, it contains 
 *        a false sharing array to gather the results of each thread
 * 
 */
struct pi_omp_args_fs { 
  int n_threads;
  long terms;
  double * pi;
};

/**
 * @brief Struct to pass arguments to omp, it contains 
 *        a true sharing array to gather the results of each thread
 * 
 */
struct pi_omp_args_ts { 
  int n_threads;
  long terms;
  double ** pi;
};

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


static void * parallel_pi_approx_fs(const int n_threads, const long terms, double pi[n_threads])
{
  #pragma omp parallel num_threads(n_threads)
  {
    thread_id_t thread_id = omp_get_thread_num();

    long chunk = terms / n_threads;
    long from = thread_id * terms;
    long to = from + chunk - 1;

    double current_pi = pi_approx(from, to);

    *(pi + thread_id) = current_pi;
  }

  return ( void * ) pi;
}

static void ** parallel_pi_approx_ts(const int n_threads, const long terms, double pi[n_threads][PAD])
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

  return ( void ** ) pi;
}

static void * parallel_pi_wrapper_fs(void * args)
{
  struct pi_omp_args_fs * args_struct = (struct pi_omp_args_fs *) args;

  return parallel_pi_approx_fs(args_struct -> n_threads, args_struct -> terms, args_struct -> pi);
}

static void * parallel_pi_wrapper_ts(void * args)
{
  struct pi_omp_args_ts * args_struct = (struct pi_omp_args_ts *) args;

  return parallel_pi_approx_ts(args_struct -> n_threads, args_struct -> terms, args_struct -> pi);
}

int main(void)
{

  const long N = 2e9;
  const int n_threads[6] = {1, 2, 4, 8, 12, 16};

  double benchmarks[2][6] = {{0.0}}; // 6 tests for 1, 2, 4, 8, 12 and 16 threads

  printf("Starting benchmarks...\n");
  for (int i = 0; i < 6; i++)
  {
    double pi_arr_fs[i];
    struct pi_omp_args_fs args_fs = {n_threads[i], N, pi_arr_fs};

    benchmarks[0][i] = measure_time(parallel_pi_wrapper_fs, (void *) &args_fs, NULL);
    printf("Finished test with %2d thread(s) with false sharing \n", n_threads[i]);

  }

  for (int i = 0; i < 6; i++)
  {
    double pi_arr_ts[i][PAD];
    struct pi_omp_args_ts args_ts = {n_threads[i], N, pi_arr_ts};

    benchmarks[1][i] = measure_time(parallel_pi_wrapper_ts, (void *) &args_ts, NULL);
    printf("Finished test with %2d thread(s) with true sharing \n", n_threads[i]);

  }

  printf("\nFalse sharing benchmarks:\n");
  for (int i = 0; i < 6; i++)
  {
    printf("Threads: %2d, Time: %f\n", n_threads[i], benchmarks[0][i]);
  }


  printf("\nTrue sharing benchmarks:\n");
  for (int i = 0; i < 6; i++)
  {
    printf("Threads: %2d, Time: %f\n", n_threads[i], benchmarks[1][i]);
  }


  return EXIT_SUCCESS;
}
