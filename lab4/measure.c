#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#include "points.h"
#include "kdtree.h"

#define TT 8
#define MM 4
#define MM_FOR_INSERT 1000000
#define MM_FOR_FIND 1000000

int main() {
  srand(time(NULL));
  omp_set_nested(1);
  
  const int T[TT] = {1, 2, 3, 4, 5, 6, 7, 8};
  const int M[MM] = {100000, 200000, 400000, 800000};

  int DIM = 2;
  float DEV = 8*8;
  
  printf("\n\nCreation\n\n");
  FILE* output = fopen("create.csv", "w");
  fprintf(output, "points_amount,single");
  for (int t = 0; t < TT; t++) {
    fprintf(output, ",%d_thr", T[t]);
  }
  fprintf(output, "\n");

  double start,end;
  
  for (int m = 0; m < MM; m++) {
    float** points;
    
    points = gen_random_points(M[m], DIM, DEV);
    start = omp_get_wtime();
    Node* single = grow_tree(points, M[m], DIM, 0);
    end = omp_get_wtime();
    free_points(points, M[m]);
    
    printf("create single: %f\n", end-start);
    fprintf(output, "%d,%f", M[m], end-start);
                   
    for (int t = 0; t < TT; t++) {
      omp_set_num_threads(T[t]);
      
      printf("Points: %d. Threads: %d.\n", M[m], T[t]);

      double times[3];
      for (int i = 0; i < 3; i++) {
        points = gen_random_points(M[m], DIM, DEV);
        start = omp_get_wtime();
        Node* parallel = grow_tree_parallel(points, M[m], DIM, 0);
        end = omp_get_wtime();
        free_points(points, M[m]);
        
        times[i] = end-start;
        for (int i = 0; i < M[m]; i++) {
          // assert(S[i]==P[i]);
        }
        free(parallel);
      }
      printf("create parallel: %f\n", (times[0]+times[1]+times[2])/3);
      fprintf(output, ",%f", (times[0]+times[1]+times[2])/3);
    }
    fprintf(output, "\n");
    free_tree(single);
  }
  fclose(output);
  
  printf("\n\nInsertion\n\n");
   
  output = fopen("insert.csv", "w");
  fprintf(output, "points_amount,single");
  for (int t = 0; t < TT; t++) {
    fprintf(output, ",%d_thr", T[t]);
  }
  fprintf(output, "\n");
  
  float** points = gen_random_points(MM_FOR_INSERT, DIM, DEV);
  Node* root = grow_tree(points, MM_FOR_INSERT, DIM, 0);

  double time = 0;
  float* point;
  for (int measure = 0; measure < MM_FOR_INSERT; measure++) {
    point = gen_random_point(DIM, DEV);
    start = omp_get_wtime();
    root = insert_point(root, point, DIM, 0);
    end = omp_get_wtime();
    time += end-start;
  }
  printf("insert single (x%d): %f\n", MM_FOR_INSERT, time);
  fprintf(output, "%d,%f", MM_FOR_INSERT, time);
  free_tree(root);
  
  
  
  printf("\n\nSearch\n\n");
   
  output = fopen("search.csv", "w");
  fprintf(output, "points_amount,single");
  for (int t = 0; t < TT; t++) {
    fprintf(output, ",%d_thr", T[t]);
  }
  fprintf(output, "\n");
  
  points = gen_random_points(MM_FOR_FIND, DIM, DEV);
  root = grow_tree(points, MM_FOR_FIND, DIM, 0);

  time = 0;
  for (int measure = 0; measure < MM_FOR_FIND; measure++) {
    point = gen_random_point(DIM, DEV);
    start = omp_get_wtime();
    root = insert_point(root, point, DIM, 0);
    end = omp_get_wtime();
    time += end-start;
  }
  printf("search single (x%d): %f\n", MM_FOR_FIND, time);
  fprintf(output, "%d,%f", MM_FOR_FIND, time);
   
  return 0;
}
