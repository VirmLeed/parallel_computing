#include <stdio.h>
#include <assert.h>
#include <omp.h>
#include "mult.c"

#define TT 5
#define MM 10
#define RANDOM_MAX 64000

int main() {
  const int T[TT] = {2, 4, 8, 10, 64};
  const int M[MM] = {500, 550, 600, 650, 700, 750, 800, 850, 900, 950, 1000, 1050, 1150, 1200, 1250};

  double start;
  double end;

  FILE* output = fopen("data.csv", "w");
  fprintf(output, "matrix_side,classic_single,single_time,2_time,4_time,8_time,10_time,64_time\n");

  for (int m = 0; m < MM; m++) {
    matrix* A = create_matrix(M[m], M[m]);
    matrix* B = create_matrix(M[m], M[m]);
    fill_matrix(A, RANDOM_MAX);
    fill_matrix(B, RANDOM_MAX);
    double times[3];

    printf("Matrix side: %d. Single threaded.\n", M[m]);
    mult_matrix(A, B);
    mult_matrix(A, B);

    start = omp_get_wtime();
    matrix* S = mult_matrix(A, B);
    end = omp_get_wtime();

    printf("%f\n", end-start);
    fprintf(output, "%d,%f", M[m], end-start);

    printf("Matrix side: %d. Single threaded classic.\n", M[m]);
    mult_matrix_classic(A, B);
    mult_matrix_classic(A, B);

    start = omp_get_wtime();
    matrix* C = mult_matrix(A, B);
    end = omp_get_wtime();
    assert(are_equal_matrix(S, C));

    printf("%f\n", end-start);
    fprintf(output, ",%f", end-start);

    for (int t = 0; t < TT; t++) {
      printf("Matrix side: %d. Threads: %d.\n", M[m], T[t]);
      mult_matrix_parallel(A, B, T[t]);
      mult_matrix_parallel(A, B, T[t]);

      for (int i = 0; i < 3; i++) {
        start = omp_get_wtime();
        matrix* P = mult_matrix_parallel(A, B, T[t]);
        end = omp_get_wtime();
        times[i] = end-start;
        assert(are_equal_matrix(S, P));
        free_matrix(P);
      }
      fprintf(output, ",%f", (times[0]+times[1]+times[2])/3);
    }
    fprintf(output, "\n");
    free_matrix(S);
    free_matrix(A);
    free_matrix(B);
  }
  fclose(output);
  
  return 0;
}
