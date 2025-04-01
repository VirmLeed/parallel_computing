
#include <stdio.h>
#include <assert.h>
#include <omp.h>
#include "mult.c"

#define TT 5
#define MM 5
#define RANDOM_MAX 64000

int main() {
  const int T[TT] = {2, 4, 8, 10, 64};
  const int M[MM] = {500, 550, 600, 650, 700};//, 1000, 1050, 1150, 1200, 1250};

  double start;
  double end;

  FILE* output = fopen("data2.csv", "w");
  fprintf(output, "matrix_side,2_time,2_time_atomic,2_time_mutex,2_time_mutex_arr,4_time,4_time_atomic,4_time_mutex,4_time_mutex_arr,8_time,8_time_atomic,8_time_mutex,8_time_mutex_arr,10_time,10_time_atomic,10_time_mutex,10_time_mutex_arr,64_time,64_time_atomic,64_time_mutex,64_time_mutex_arr\n");

  for (int m = 0; m < MM; m++) {
    matrix* A = create_matrix(M[m], M[m]);
    matrix* B = create_matrix(M[m], M[m]);
    fill_matrix(A, RANDOM_MAX);
    fill_matrix(B, RANDOM_MAX);
    double times[3];

    matrix* S = mult_matrix(A, B);
    for (int t = 0; t < TT; t++) {
      printf("Matrix side: %d. Threads: %d.\n", M[m], T[t]);
      printf("parallel\n");
      free(mult_matrix_parallel(A, B, T[t]));
      free(mult_matrix_parallel(A, B, T[t]));

      for (int i = 0; i < 3; i++) {
        start = omp_get_wtime();
        matrix* P = mult_matrix_parallel(A, B, T[t]);
        end = omp_get_wtime();
        times[i] = end-start;
        assert(are_equal_matrix(S, P));
        free_matrix(P);
      }
      fprintf(output, ",%f", (times[0]+times[1]+times[2])/3);

      printf("atomic\n");
     free_matrix_atomic(mult_matrix_parallel_race_atomic(A, B, T[t]));
     free_matrix_atomic(mult_matrix_parallel_race_atomic(A, B, T[t]));

      for (int i = 0; i < 3; i++) {
        start = omp_get_wtime();
        matrix_atomic* P = mult_matrix_parallel_race_atomic(A, B, T[t]);
        end = omp_get_wtime();
        times[i] = end-start;
        assert(compare_matrix_to_atomic(S, P));
        free_matrix_atomic(P);
      }
      fprintf(output, ",%f", (times[0]+times[1]+times[2])/3);

      printf("mutex\n");
      free(mult_matrix_parallel_race_mutex(A, B, T[t]));
      free(mult_matrix_parallel_race_mutex(A, B, T[t]));

      for (int i = 0; i < 3; i++) {
        start = omp_get_wtime();
        matrix* P = mult_matrix_parallel_race_mutex(A, B, T[t]);
        end = omp_get_wtime();
        times[i] = end-start;
        assert(are_equal_matrix(S, P));
        free_matrix(P);
      }
      fprintf(output, ",%f", (times[0]+times[1]+times[2])/3);
      printf("mutex arrays\n");

      free(mult_matrix_parallel_race_mutex_arr(A, B, T[t]));
      free(mult_matrix_parallel_race_mutex_arr(A, B, T[t]));

      for (int i = 0; i < 3; i++) {
        start = omp_get_wtime();
        matrix* P = mult_matrix_parallel_race_mutex_arr(A, B, T[t]);
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
