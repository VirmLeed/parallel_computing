#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include "bfm.h"
#include "matrix.h"

#define WIDTH 20
#define HEIGHT 30

int main() {
  matrix* A = create_matrix(WIDTH, HEIGHT);
  matrix* B = create_matrix(WIDTH, HEIGHT);

  printf("filling matrices\n");
  fill_matrix(A);
  fill_matrix(B);

  printf("brute-force matching\n");
  int* result = match(A, B);

  printf("freeing matrices\n");
  free_matrix(A);
  free_matrix(B);

  printf("result:\n");
  for (int i = 0; i < HEIGHT; i++) {
    printf("%d\n", result[i]);
  }
  free(result);

  return 0;
}
