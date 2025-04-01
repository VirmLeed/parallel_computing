#ifndef MATRIX_H
#define MATRIX_H

typedef struct {
  float** v;
  int w;
  int h;
} matrix;

matrix* create_matrix(int width, int height);
matrix* fill_matrix(matrix* input);
matrix* clone_matrix(matrix* input);
void free_matrix(matrix* input);
void print_matrix(matrix* input);
int are_equal_matrix(matrix* A, matrix* B);

#endif
