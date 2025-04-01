#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <assert.h>
#include "bfm.h"
#include "matrix.h"

#define WIDTH 200*3
#define HEIGHT 300*3

int main() {
  omp_set_num_threads(8);
  // omp_set_nested(1);
  double start;
  double end;
  
  printf("allocating matrices\n");
  matrix* A = create_matrix(WIDTH, HEIGHT);
  matrix* B = create_matrix(WIDTH, HEIGHT);

  // printf("printing matrices\n");
  // print_matrix(A);
  // print_matrix(B);

  printf("filling matrices\n");
  fill_matrix(A);
  fill_matrix(B);

  // printf("printing matrices\n");
  // print_matrix(A);
  // print_matrix(B);

  printf("brute-force matching\n");
  start = omp_get_wtime();
  int* result = match(A, B);
  end = omp_get_wtime();
  printf("took %f\n", end-start);

  printf("parallel matching\n");
  start = omp_get_wtime();
  int* result2 = match_parallel(A, B);
  end = omp_get_wtime();
  printf("took %f\n", end-start);

  for (int i = 0; i < HEIGHT; i++)
    assert(result[i] == result2[i]);

  printf("freeing matrices\n");
  free_matrix(A);
  free_matrix(B);

  // printf("result:\n");
  // for (int i = 0; i < HEIGHT; i++) {
    // printf("%d\n", result[i]);
  // }

  free(result);
  free(result2);

  return 0;
}
