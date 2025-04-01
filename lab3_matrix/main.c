#include <stdio.h>
#include <omp.h>
#include "matrix.c"

#define WIDTH 20
#define HEIGHT 30

int main() {
  matrix* A = create_matrix(WIDTH, HEIGHT);
  matrix* B = create_matrix(WIDTH, HEIGHT);

  printf("filling matrices\n");
  fill_matrix(A);
  fill_matrix(B);
  
  return 0;
}
