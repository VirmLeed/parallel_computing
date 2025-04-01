#include <stdatomic.h>
#include <stdio.h>
#include <assert.h>
#include <omp.h>
#include "mult.c"

/*
// По очереди считать каждый элемент итоговой матрицы.
// Для параллельности просто каждый поток считает свои элементы итоговой матрицы.
matrix* mult_matrix(matrix* A, matrix* B) {
  assert(A->w == B->h);
  matrix* output = create_matrix(B->w, A->h);
  int sum;

  for (int i = 0; i < output->l; i++) {
    sum = 0;
    for (int sumele = 0; sumele < A->w; sumele++)
      sum  += A->v[(i/B->w)*A->w + sumele] * B->v[i%B->w + sumele*B->w];
    output->v[i] = sum;
  }

  return output;
}

// Данные, передаваемые в каждый поток
typedef struct {
  matrix* A;
  matrix* B;
  matrix* output;
  int start;
  int end;
} thread_data;

// Каждый поток считает свои элементы итоговой матрицы.
void* mult_matrix_parallel_thread(void* args) {
  thread_data* data = (thread_data*)args;
  int sum;
  
  for (int i = data->start; i < data->end; i++) {
    sum = 0;
    for (int sumele = 0; sumele < data->A->w; sumele++)
       sum += data->A->v[(i/data->B->w)*data->A->w + sumele] * data->B->v[i%data->B->w + sumele*data->B->w];
    data->output->v[i] = sum;
  }

  return 0;
}

matrix* mult_matrix_parallel(matrix* A, matrix* B, int THREAD_AMOUNT) {
  assert(A->w == B->h);
  matrix* output = create_matrix(B->w, A->h);
  assert(THREAD_AMOUNT <= output->l);
  pthread_t threads[THREAD_AMOUNT];
  thread_data data[THREAD_AMOUNT];

  int matrix_part_len = (output->l + THREAD_AMOUNT - 1) / THREAD_AMOUNT;
  // int matrix_part_len = A->w / THREAD_AMOUNT;
  int last_part_len = output->l - matrix_part_len*(THREAD_AMOUNT-1);

  for (int thread = 0; thread < THREAD_AMOUNT; thread++) {
    data[thread].A = A;
    data[thread].B = B;
    data[thread].output = output;
    data[thread].start = thread*matrix_part_len;
    if (thread+1 == THREAD_AMOUNT)
      data[thread].end = (thread)*matrix_part_len + last_part_len;
    else
      data[thread].end = (thread+1)*matrix_part_len;
    pthread_create(&threads[thread], NULL, mult_matrix_parallel_thread, (void*)&data[thread]);
  }

  for (int t = 0; t < THREAD_AMOUNT; t++)
    pthread_join(threads[t], NULL);

  return output;
}

// Функция с гонкой. 
// Каждый поток считает все элементы итоговой матрицы по одному элементу входных матриц.
// Может совпасть (учитывая то, что потоки начинают с одной стороны, скорее всего так и произойдет)
//   что несколько потоков будут пытаться одновременно прибавить к одному значению массива результата.
// Почему-то если во внутреннем цикле их складывать, а к результату сумму прибавлять уже потом, гонка не возникает
void* mult_matrix_parallel_race_thread(void* args) {
  thread_data* data = (thread_data*)args;
  int sum;

  for (int i = 0; i < data->output->l; i++) {
    sum = 0;
    for (int sumele = data->start; sumele < data->end; sumele++)
       sum += data->A->v[(i/data->B->w)*data->A->w + sumele] * data->B->v[i%data->B->w + sumele*data->B->w];
       // data->output->v[i] += data->A->v[(i/data->B->w)*data->A->w + sumele] * data->B->v[i%data->B->w + sumele*data->B->w];
    data->output->v[i] += sum;
  }

  return 0;
}

matrix* mult_matrix_parallel_race(matrix* A, matrix* B, int THREAD_AMOUNT) {
  assert(A->w == B->h);
  matrix* output = create_matrix(B->w, A->h);
  assert(THREAD_AMOUNT <= A->w);
  pthread_t threads[THREAD_AMOUNT];
  thread_data data[THREAD_AMOUNT];

  // int matrix_part_len = (A->w + THREAD_AMOUNT - 1) / THREAD_AMOUNT;
  int matrix_part_len = A->w / THREAD_AMOUNT;
  int last_part_len = A->w - matrix_part_len*(THREAD_AMOUNT-1);

  for (int thread = 0; thread < THREAD_AMOUNT; thread++) {
    data[thread].A = A;
    data[thread].B = B;
    data[thread].output = output;
    data[thread].start = thread*matrix_part_len;
    if (thread+1 == THREAD_AMOUNT)
      data[thread].end = (thread)*matrix_part_len + last_part_len;
    else
      data[thread].end = (thread+1)*matrix_part_len;

    pthread_create(&threads[thread], NULL, mult_matrix_parallel_race_thread, (void*)&data[thread]);
  }

  for (int t = 0; t < THREAD_AMOUNT; t++)
    pthread_join(threads[t], NULL);

  return output;
}
*/

int main(int argc, char** argv) {
  double start;
  double end;

  int THREAD_AMOUNT = 200;
  int MATRIX_SIZE = 1000;
  int RANDOM_MAX = 16000; //65536;

  for (int i = 1; i < argc; i++) {
    int val = atoi(argv[i]);
    if (i == 1) {
      THREAD_AMOUNT = val;
      MATRIX_SIZE = val;
    } else if (i == 2) {
      if (val*val >= THREAD_AMOUNT)
        MATRIX_SIZE = val;
    } else if (i == 3)
      RANDOM_MAX = val;
  }

  matrix* A = create_matrix(MATRIX_SIZE, MATRIX_SIZE);
  matrix* B = create_matrix(MATRIX_SIZE, MATRIX_SIZE);
  // matrix* A = create_matrix(5, 4);
  // matrix* B = create_matrix(3, 5);
  // matrix* A = create_matrix(300, 40);
  // matrix* B = create_matrix(40, 300);
  printf("filling matrices\n");
  fill_matrix(A, RANDOM_MAX);
  fill_matrix(B, RANDOM_MAX);

  printf("Running atomic thing\n");
  free(mult_matrix_parallel_race_atomic(A, B, THREAD_AMOUNT));
  free(mult_matrix_parallel_race_atomic(A, B, THREAD_AMOUNT));

  start = omp_get_wtime();
  matrix_atomic* at = mult_matrix_parallel_race_atomic(A, B, THREAD_AMOUNT);
  end = omp_get_wtime();
  printf("Calculation took %f\n", end-start);

  printf("SINGLE THREAD CLASSIC\n");
  start = omp_get_wtime();
  matrix* tmp = mult_matrix_classic(A, B);
  end = omp_get_wtime();
  printf("Calculation took %f\n", end-start);

  printf("SINGLE THREAD\n");
  printf("Running calculations two times to precache or whatever\n");
  mult_matrix(A, B);
  mult_matrix(A, B);

  printf("Running the real deal\n");
  start = omp_get_wtime();
  matrix* C = mult_matrix(A, B);
  end = omp_get_wtime();
  printf("Calculation took %f\n", end-start);

  assert(compare_matrix_to_atomic(C, at));

  // return 0;
  printf("Mutex\n");
  free(mult_matrix_parallel_race_mutex(A, B, THREAD_AMOUNT));
  free(mult_matrix_parallel_race_mutex(A, B, THREAD_AMOUNT));

  start = omp_get_wtime();
  matrix* mut = mult_matrix_parallel_race_mutex(A, B, THREAD_AMOUNT);
  end = omp_get_wtime();
  printf("Calculation took %f\n", end-start);

  assert(are_equal_matrix(C, mut));
  
  printf("RACE CONDITION\n");
  printf("Running calculations two times to precache or whatever\n");
  mult_matrix_parallel_race(A, B, THREAD_AMOUNT);
  mult_matrix_parallel_race(A, B, THREAD_AMOUNT);

  printf("Running the real deal\n");
  start = omp_get_wtime();
  matrix* E = mult_matrix_parallel_race(A, B, THREAD_AMOUNT);
  end = omp_get_wtime();
  printf("Calculation took %f\n", end-start);

  assert(are_equal_matrix(C, E));
  return 0;

  printf("MULTIPLE THREADS\n");
  printf("Running calculations two times to precache or whatever\n");
  mult_matrix_parallel(A, B, THREAD_AMOUNT);
  mult_matrix_parallel(A, B, THREAD_AMOUNT);

  printf("Running the real deal\n");
  start = omp_get_wtime();
  matrix* D = mult_matrix_parallel(A, B, THREAD_AMOUNT);
  end = omp_get_wtime();
  printf("Calculation took %f\n", end-start);
  printf("%f\n", end-start);

  assert(are_equal_matrix(C, D));

  free_matrix(A);
  free_matrix(B);
  free_matrix(C);
  free_matrix(D);
  free_matrix(E);
  
  return 0;
}

