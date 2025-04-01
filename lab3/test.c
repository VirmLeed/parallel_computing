#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <assert.h>
#include "bfm.h"
#include "matrix.h"

#define TT 8
#define MM 16

int main() {
  const int T[TT] = {1, 2, 3, 4, 5, 6, 7, 8};
  const int M[MM] = {200, 250, 300, 350, 400, 450, 500, 550, 600, 650, 700, 750, 800, 850, 900, 950};

  double start;
  double end;

  FILE* output = fopen("data.csv", "w");
  fprintf(output, "matrix_side,single");
  for (int t = 0; t < TT; t++)
    fprintf(output, ",%d_thr", T[t]);
  fprintf(output, "\n");

  for (int m = 0; m < MM; m++) {
    matrix* A = create_matrix(M[m], M[m]);
    matrix* B = create_matrix(M[m], M[m]);
    fill_matrix(A);
    fill_matrix(B);
    double times[3];

    printf("Matrix side: %d. Single threaded.\n", M[m]);
    free(match(A, B));
    free(match(A, B));

    start = omp_get_wtime();
    int* S = match(A, B);
    end = omp_get_wtime();

    printf("%f\n", end-start);
    fprintf(output, "%d,%f", M[m], end-start);

    for (int t = 0; t < TT; t++) {
      omp_set_num_threads(T[t]);
      printf("Matrix side: %d. Threads: %d.\n", M[m], T[t]);
      free(match_parallel(A, B));
      free(match_parallel(A, B));

      for (int i = 0; i < 3; i++) {
        start = omp_get_wtime();
        int* P = match_parallel(A, B);
        end = omp_get_wtime();
        times[i] = end-start;
        for (int i = 0; i < M[m]; i++) {
          assert(S[i]==P[i]);
        }
        free(P);
      }
      printf("%f\n", (times[0]+times[1]+times[2])/3);
      fprintf(output, ",%f", (times[0]+times[1]+times[2])/3);
    }
    fprintf(output, "\n");
    free(S);
    free_matrix(A);
    free_matrix(B);
  }
  fclose(output);
  
  return 0;
}
