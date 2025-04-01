#include <stdio.h>
#include <assert.h>
#include <omp.h>
#include "mult.c"

#define TT 8
#define MM 8
#define RANDOM_MAX 64000
#define SAMPLE_SIZE 50

double mean(double* arr, int len) {
  double sum = 0;
  for (int i = 0; i < len; i++) {
    sum += arr[i];
  }
  return sum/len;
}

int main() {
  const int T[TT] = {1, 2, 3, 4, 5, 6, 7, 8};
  const int M[MM] = {50, 100, 150, 200, 250, 300, 350, 400};//, 1000, 1050, 1150, 1200, 1250};

  double start;
  double end;

  FILE* output = fopen("racism.csv", "w");
  fprintf(output, "matrix_side,1_success,2_success,3_success,4_success,5_success,6_success,7_success,8_success\n");

  for (int m = 0; m < MM; m++) {
    matrix* A = create_matrix(M[m], M[m]);
    matrix* B = create_matrix(M[m], M[m]);
    fill_matrix(A, RANDOM_MAX);
    fill_matrix(B, RANDOM_MAX);
    double times[SAMPLE_SIZE];

    matrix* S = mult_matrix(A, B);
    fprintf(output, "%d", m);
    for (int t = 0; t < TT; t++) {
      printf("Matrix side: %d. Threads: %d.\n", M[m], T[t]);
      mult_matrix_parallel_race(A, B, T[t]);
      mult_matrix_parallel_race(A, B, T[t]);

      for (int i = 0; i < SAMPLE_SIZE; i++) {
        start = omp_get_wtime();
        matrix* P = mult_matrix_parallel_race(A, B, T[t]);
        end = omp_get_wtime();
        times[i] = are_equal_matrix(S, P);
        free_matrix(P);
      }
      printf("%f\n", mean(times, SAMPLE_SIZE));
      fprintf(output, ",%f", mean(times, SAMPLE_SIZE));
    }
    fprintf(output, "\n");
    free_matrix(S);
    free_matrix(A);
    free_matrix(B);
  }
  fclose(output);
  
  return 0;
}
