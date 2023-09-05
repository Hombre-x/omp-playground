#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

// Single threaded
static double pi_approx_v1(int n)
{

  double pi = 0.0;
  int sign = 1;

  for (int i = 0; i <= n; i++)
  {
    pi += sign / (2.0 * i + 1.0);
    sign *= -1;
  }

  return 4.0 * pi;
}

// Multi threaded

struct pi_arguments {
  int n;
  double half_pi;
};

static double pi_approx_even(int n)
{
  double half_pi = 0.0;
  int sign = 1;

  for (int i = 0; i <= n; i += 2)
  {
    half_pi += sign / (2.0 * i + 1.0);
  }

  return half_pi;
}

static double pi_approx_odd(int n)
{
  double half_pi = 0.0;
  int sign = -1;

  for (int i = 1; i <= n; i += 2)
  {
    half_pi += sign / (2.0 * i + 1.0);
  }

  return half_pi;
}

static void * wrapper_pi_approx_even(void * arg)
{
  struct pi_arguments * args = (struct pi_arguments *) arg;
  double half_pi = pi_approx_even(args -> n);

  args -> half_pi = half_pi;

  pthread_exit((void *) &(args -> half_pi));

}

static void * wrapper_pi_approx_odd(void * arg)
{
  struct pi_arguments * args = (struct pi_arguments *) arg;
  double half_pi = pi_approx_odd(args -> n);

  args -> half_pi = half_pi;

  pthread_exit((void *) &(args -> half_pi));

}

static void test2(void)
{
  pthread_t even_thread = 0, odd_thread = 0;

  const int number_of_terms = 2e9;

  static struct pi_arguments args_even = {number_of_terms, 0.0}, args_odd = {number_of_terms, 0.0};

  double half_pi_even = 0.0, half_pi_odd = 0.0;
  double * half_pi_even_ptr = &half_pi_even, * half_pi_odd_ptr = &half_pi_odd;

  int ret_even = pthread_create(&even_thread, NULL, wrapper_pi_approx_even, (void *) &args_even);
  int ret_odd  = pthread_create(&odd_thread, NULL, wrapper_pi_approx_odd, (void *) &args_odd);

  if (ret_even != 0 || ret_odd != 0)
  {
    printf("Error creating threads\n");
    exit(EXIT_FAILURE);
  }

  pthread_join(even_thread, (void *) &half_pi_even_ptr);
  pthread_join(odd_thread, (void *) &half_pi_odd_ptr);

  printf("Pi approximation with %d terms: %0.11f\n", number_of_terms, 4.0 * (*half_pi_even_ptr + *half_pi_odd_ptr));
  printf("Pi real value: %0.11f\n", M_PI);
  printf("Error: %0.11f\n", fabs(M_PI - 4.0 * (*half_pi_even_ptr + *half_pi_odd_ptr)));

}

static void test1(void)
{
  printf("Pi approximation with 10 terms: %0.11f\n", pi_approx_v1(10));
  printf("Pi approximation with 100 terms: %0.11f\n", pi_approx_v1(100));
  printf("Pi approximation with 1000 terms: %0.11f\n", pi_approx_v1(1000));
  printf("Pi approxximation with 100000000 terms: %0.11f\n", pi_approx_v1(100000000));
  printf("Pi real value: %0.11f\n", M_PI);
}

int main(void)
{

  test2();

  return EXIT_SUCCESS;
}
