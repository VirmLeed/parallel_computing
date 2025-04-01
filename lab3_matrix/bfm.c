#include <math.h>
#include <assert.h>
#include "matrix.c"

float* match(matrix* A, matrix* B) {
  assert(A->w == B->w);
  assert(A->h == B->h);
  float* output = malloc(sizeof(float) * A->h);

  for (int y = 0; y < A->h; y++) {
    float* A_row = A->v[y];
    float min = INT32_MAX;

    for (int other_y = 0; other_y < A->h; other_y++) {
      int sum = 0;
      for (int x = 0; x < A->w; x++)
        sum += pow(B->v[x][y], 2);

      if (sum < min)
        min = sum;
    }

    output[y] = min;
  }

  return output;
}
