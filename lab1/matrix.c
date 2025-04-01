#include <stdlib.h>
#include <stdio.h>
#include <stdatomic.h>

typedef struct {
  int* v;
  int w;
  int h;
  int l;
} matrix;

matrix* create_matrix(int width, int height) {
  matrix* output = malloc(sizeof(matrix));
  output->w = width;
  output->h = height;
  output->l = width*height;
  output->v = malloc(sizeof(int) * output->l);

  for (int i = 0; i < output->l; i++)
    output->v[i] = 0;

  return output;
}

matrix* fill_matrix(matrix* input, int max_rand) {
  for (int i = 0; i < input->l; i++)
    input->v[i] = rand() % max_rand;

  return input;
}

matrix* clone_matrix(matrix* input) {
  matrix* output = create_matrix(input->w, input->h);

  for (int i = 0; i < input->l; i++)
    output->v[i] = input->v[i];

  return output;
}

void free_matrix(matrix* input) {
  free(input->v);
  free(input);
}

int are_equal_matrix(matrix* A, matrix* B) {
  if (A->w != B->w && A->h != B->h && A->l != B->l)
    return 0;

  for (int i = 0; i < A->l; i++)
    if (A->v[i] != B->v[i])
      return 0;

  return 1;
}

void print_matrix(matrix* input) {
  for (int y = 0; y < input->h; y++) {
    for (int x = 0; x < input->w; x++)
      printf("%d ", input->v[x+y*input->w]);
    printf("\n");
  }
  printf("\n");
}

typedef struct {
  atomic_int* v;
  int w;
  int h;
  int l;
} matrix_atomic;

matrix_atomic* create_matrix_atomic(int width, int height) {
  matrix_atomic* output = malloc(sizeof(matrix));
  output->w = width;
  output->h = height;
  output->l = width*height;
  output->v = malloc(sizeof(atomic_int) * output->l);

  for (int i = 0; i < output->l; i++)
    output->v[i] = 0;

  return output;
}

matrix_atomic* fill_matrix_atomic(matrix_atomic* input, int max_rand) {
  for (int i = 0; i < input->l; i++)
    atomic_init(&input->v[i], rand() % max_rand);

  return input;
}

void free_matrix_atomic(matrix_atomic* input) {
  free(input->v);
  free(input);
}

int compare_matrix_to_atomic(matrix* A, matrix_atomic* B) {
    if (A->w != B->w && A->h != B->h && A->l != B->l)
    return 0;

  for (int i = 0; i < A->l; i++) {
    int b_val = atomic_load(&B->v[i]);
    if (A->v[i] != b_val)
      return 0;
  }

  return 1;
}
