#include <stdlib.h>
#include <stdatomic.h>

typedef struct {
  float** v;
  int w;
  int h;
} matrix;

matrix* create_matrix(int width, int height) {
  matrix* output = malloc(sizeof(matrix));
  output->v = malloc(sizeof(float)*width*height);

  for (int y = 0; y < height; y++)
    for (int x = 0; x < width; x++)
      output->v[x][y] = 0;

  return output;
}

matrix* fill_matrix(matrix* input) {
  for (int y = 0; y < input->h; y++)
    for (int x = 0; x < input->w; x++)
      input->v[x][y] = (float)rand() / RAND_MAX;

  return input;
}

matrix* clone_matrix(matrix* input) {
  matrix* output = create_matrix(input->w, input->h);

  for (int y = 0; y < input->h; y++)
    for (int x = 0; x < input->w; x++)
      output->v[x][y] = input->v[x][y];

  return output;
}

void free_matrix(matrix* input) {
  free(input->v);
  free(input);
}

int are_equal_matrix(matrix* A, matrix* B) {
  if (A->w != B->w && A->h != B->h)
    return 0;

  for (int y = 0; y < B->h; y++)
    for (int x = 0; x < A->w; x++)
      if (A->v[x][y] != B->v[x][y])
        return 0;

  return 1;
}
