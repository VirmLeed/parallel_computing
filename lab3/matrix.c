#include "matrix.h"
#include <stdlib.h>
#include <stdio.h>

matrix* create_matrix(int width, int height) {
  matrix* output = malloc(sizeof(matrix));
  output->v = malloc(sizeof(float*)*height);
  for (int i = 0; i < height; i++)
    output->v[i] = malloc(sizeof(float)*width);
  output->w = width;
  output->h = height;

  for (int y = 0; y < height; y++)
    for (int x = 0; x < width; x++)
      output->v[y][x] = 0;

  return output;
}

matrix* fill_matrix(matrix* input) {
  for (int y = 0; y < input->h; y++)
    for (int x = 0; x < input->w; x++)
      input->v[y][x] = (float)rand() / RAND_MAX;

  return input;
}

matrix* clone_matrix(matrix* input) {
  matrix* output = create_matrix(input->w, input->h);

  for (int y = 0; y < input->h; y++)
    for (int x = 0; x < input->w; x++)
      output->v[y][x] = input->v[x][y];

  return output;
}

void free_matrix(matrix* input) {
  free(input->v);
  free(input);
}

void print_matrix(matrix* input) {
  for (int y = 0; y < input->h; y++) {
    for (int x = 0; x < input->w; x++)
      printf("%f ", input->v[y][x]);
    printf("\n");
  }
  printf("\n");
}

int are_equal_matrix(matrix* A, matrix* B) {
  if (A->w != B->w && A->h != B->h)
    return 0;

  for (int y = 0; y < B->h; y++)
    for (int x = 0; x < A->w; x++)
      if (A->v[y][x] != B->v[y][x])
        return 0;

  return 1;
}
