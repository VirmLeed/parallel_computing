#include <assert.h>
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <stdatomic.h>
#include <omp.h>
#include "matrix.c"

// По очереди считать каждый элемент итоговой матрицы.
matrix* mult_matrix(matrix* A, matrix* B) {
  assert(A->w == B->h);
  matrix* output = create_matrix(B->w, A->h);
  int sum;
  int y;
  int x;

  for (int i = 0; i < output->l; i++) {
    sum = 0;
    y = (i/B->w)*A->w;
    x = i%B->w;
    for (int sumele = 0; sumele < A->w; sumele++)
      sum  += A->v[y + sumele] * B->v[x + sumele*B->w];
    output->v[i] = sum;
  }

  return output;
}

// По очереди считать каждый элемент итоговой матрицы.
matrix* mult_matrix_classic(matrix* A, matrix* B) {
  assert(A->w == B->h);
  matrix* output = create_matrix(B->w, A->h);
  int sum;

  for (int y = 0; y < A->h; y++) {
    for (int x = 0; x < B->w; x++) {
      sum = 0;
      for (int sumele = 0; sumele < A->w; sumele++)
        sum+= A->v[y + sumele] * B->v[x + sumele*B->w];
      output->v[y*B->w + x] = sum;
    }
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
  int y;
  int x;
  
  for (int i = data->start; i < data->end; i++) {
    sum = 0;
    y = (i/data->B->w)*data->A->w;
    x = i%data->B->w;
    for (int sumele = 0; sumele < data->A->w; sumele++)
      sum += data->A->v[y + sumele] * data->B->v[x + sumele*data->B->w];
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
  if ((matrix_part_len * (THREAD_AMOUNT-1)) > output->l) {
    matrix_part_len = A->w / THREAD_AMOUNT;
  }
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
// Для избежания этого в итоговой матрице элементы атомарные
void* mult_matrix_parallel_race_thread(void* args) {
  thread_data* data = (thread_data*)args;
  int sum;
  int y;
  int x;

  for (int i = 0; i < data->output->l; i++) {
    sum = 0;
    y = (i/data->B->w)*data->A->w;
    x = i%data->B->w;
    for (int sumele = data->start; sumele < data->end; sumele++)
      sum += data->A->v[y + sumele] * data->B->v[x + sumele*data->B->w];
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

// Данные, передаваемые в каждый поток
typedef struct {
  matrix* A;
  matrix* B;
  matrix_atomic* output;
  int start;
  int end;
} thread_data_atomic;

// Функция с гонкой. 
// Каждый поток считает все элементы итоговой матрицы по одному элементу входных матриц.
// Может совпасть (учитывая то, что потоки начинают с одной стороны, скорее всего так и произойдет)
//   что несколько потоков будут пытаться одновременно прибавить к одному значению массива результата.
// Для избежания этого в итоговой матрице элементы атомарные
void* mult_matrix_parallel_race_thread_atomic(void* args) {
  thread_data_atomic* data = (thread_data_atomic*)args;
  int sum;
  int y;
  int x;

  for (int i = 0; i < data->output->l; i++) {
    sum = 0;
    y = (i/data->B->w)*data->A->w;
    x = i%data->B->w;

    for (int sumele = data->start; sumele < data->end; sumele++)
      // atomic_fetch_add(&data->output->v[i], data->A->v[y + sumele] * data->B->v[x + sumele*data->B->w]);
      sum += data->A->v[y + sumele] * data->B->v[x + sumele*data->B->w];
    atomic_fetch_add(&data->output->v[i], sum);
  }

  return 0;
}

matrix_atomic* mult_matrix_parallel_race_atomic(matrix* A, matrix* B, int THREAD_AMOUNT) {
  assert(A->w == B->h);
  matrix_atomic* output = create_matrix_atomic(B->w, A->h);
  assert(THREAD_AMOUNT <= A->w);
  pthread_t threads[THREAD_AMOUNT];
  thread_data_atomic data[THREAD_AMOUNT];

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

    pthread_create(&threads[thread], NULL, mult_matrix_parallel_race_thread_atomic, (void*)&data[thread]);
  }

  for (int t = 0; t < THREAD_AMOUNT; t++)
    pthread_join(threads[t], NULL);

  return output;
}

// Данные, передаваемые в каждый поток
typedef struct {
  matrix* A;
  matrix* B;
  matrix* output;
  int start;
  int end;
  pthread_mutex_t* lock;
} thread_data_mutex;

// Функция с гонкой. 
// Каждый поток считает все элементы итоговой матрицы по одному элементу входных матриц.
// Может совпасть (учитывая то, что потоки начинают с одной стороны, скорее всего так и произойдет)
//   что несколько потоков будут пытаться одновременно прибавить к одному значению массива результата.
// Для избежания этого сумма лочится мьютексами
void* mult_matrix_parallel_race_thread_mutex(void* args) {
  thread_data_mutex* data = (thread_data_mutex*)args;
  int sum;
  int y;
  int x;

  for (int i = 0; i < data->output->l; i++) {
    sum = 0;
    y = (i/data->B->w)*data->A->w;
    x = i%data->B->w;
    for (int sumele = data->start; sumele < data->end; sumele++)
      sum += data->A->v[y + sumele] * data->B->v[x + sumele*data->B->w];
       // data->output->v[i] += data->A->v[(i/data->B->w)*data->A->w + sumele] * data->B->v[i%data->B->w + sumele*data->B->w];
    pthread_mutex_lock(data->lock);
    data->output->v[i] += sum;
    pthread_mutex_unlock(data->lock);
  }

  return 0;
}

matrix* mult_matrix_parallel_race_mutex(matrix* A, matrix* B, int THREAD_AMOUNT) {
  assert(A->w == B->h);
  matrix* output = create_matrix(B->w, A->h);
  assert(THREAD_AMOUNT <= A->w);
  pthread_t threads[THREAD_AMOUNT];
  thread_data_mutex data[THREAD_AMOUNT];
  pthread_mutex_t lock;

  pthread_mutex_init(&lock, NULL);

  // int matrix_part_len = (A->w + THREAD_AMOUNT - 1) / THREAD_AMOUNT;
  int matrix_part_len = A->w / THREAD_AMOUNT;
  int last_part_len = A->w - matrix_part_len*(THREAD_AMOUNT-1);

  for (int thread = 0; thread < THREAD_AMOUNT; thread++) {
    data[thread].A = A;
    data[thread].B = B;
    data[thread].output = output;
    data[thread].start = thread*matrix_part_len;
    data[thread].lock = &lock;
    if (thread+1 == THREAD_AMOUNT)
      data[thread].end = (thread)*matrix_part_len + last_part_len;
    else
      data[thread].end = (thread+1)*matrix_part_len;

    pthread_create(&threads[thread], NULL, mult_matrix_parallel_race_thread_mutex, (void*)&data[thread]);
  }

  for (int t = 0; t < THREAD_AMOUNT; t++)
    pthread_join(threads[t], NULL);

  pthread_mutex_destroy(&lock);

  return output;
}

// Данные, передаваемые в каждый поток
typedef struct {
  matrix* A;
  matrix* B;
  matrix* output;
  int start;
  int end;
  pthread_mutex_t* lock;
} thread_data_mutex_arr;

// Функция с гонкой. 
// Каждый поток считает все элементы итоговой матрицы по одному элементу входных матриц.
// Может совпасть (учитывая то, что потоки начинают с одной стороны, скорее всего так и произойдет)
//   что несколько потоков будут пытаться одновременно прибавить к одному значению массива результата.
// Для избежания этого сумма лочится мьютексами
void* mult_matrix_parallel_race_thread_mutex_arr(void* args) {
  thread_data_mutex_arr* data = (thread_data_mutex_arr*)args;
  int sum;
  int y;
  int x;

  for (int i = 0; i < data->output->l; i++) {
    sum = 0;
    y = (i/data->B->w)*data->A->w;
    x = i%data->B->w;
    for (int sumele = data->start; sumele < data->end; sumele++)
      sum += data->A->v[y + sumele] * data->B->v[x + sumele*data->B->w];
       // data->output->v[i] += data->A->v[(i/data->B->w)*data->A->w + sumele] * data->B->v[i%data->B->w + sumele*data->B->w];
    pthread_mutex_lock(&data->lock[i]);
    data->output->v[i] += sum;
    pthread_mutex_unlock(&data->lock[i]);
  }

  return 0;
}

matrix* mult_matrix_parallel_race_mutex_arr(matrix* A, matrix* B, int THREAD_AMOUNT) {
  assert(A->w == B->h);
  matrix* output = create_matrix(B->w, A->h);
  assert(THREAD_AMOUNT <= A->w);
  pthread_t threads[THREAD_AMOUNT];
  thread_data_mutex_arr data[THREAD_AMOUNT];
  pthread_mutex_t* lock = malloc(sizeof(pthread_mutex_t) * output->l);

  double start = omp_get_wtime();
  for (int i = 0; i < output->l; i++){
    if (pthread_mutex_init(lock + i, NULL))
      printf("error");
    }
  double end = omp_get_wtime();
  printf("Took time to init mutexes: %f", end-start);

  // int matrix_part_len = (A->w + THREAD_AMOUNT - 1) / THREAD_AMOUNT;
  int matrix_part_len = A->w / THREAD_AMOUNT;
  int last_part_len = A->w - matrix_part_len*(THREAD_AMOUNT-1);

  for (int thread = 0; thread < THREAD_AMOUNT; thread++) {
    data[thread].A = A;
    data[thread].B = B;
    data[thread].output = output;
    data[thread].start = thread*matrix_part_len;
    data[thread].lock = lock;
    if (thread+1 == THREAD_AMOUNT)
      data[thread].end = (thread)*matrix_part_len + last_part_len;
    else
      data[thread].end = (thread+1)*matrix_part_len;

    pthread_create(&threads[thread], NULL, mult_matrix_parallel_race_thread_mutex_arr, (void*)&data[thread]);
  }

  for (int t = 0; t < THREAD_AMOUNT; t++)
    pthread_join(threads[t], NULL);

  for (int i = 0; i < output->l; i++)
    pthread_mutex_destroy(&lock[i]);

  return output;
}

