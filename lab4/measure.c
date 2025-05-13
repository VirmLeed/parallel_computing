#include <assert.h>
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
  omp_set_nested(1);
  omp_set_num_threads(4);
    
  
  // const int T[TT] = {1, 2, 3, 4, 5, 6, 7, 8};
  const int M[MM] = {100000, 200000, 400000, 800000};

  int DIM = 2;
  float DEV = 8;
  int SEED = 2;

  double start,end;
  FILE* output;
  
  printf("\n\nCreation\n\n");
  output = fopen("create.csv", "w");
  fprintf(output, "points_amount,serial,threaded\n");
  
  for (int m = 0; m < MM; m++) {
    float** points;
    
    srand(SEED);
    points = gen_random_points(M[m], DIM, DEV);
    start = omp_get_wtime();
    Node* single = grow_tree(points, M[m], DIM, 0);
    end = omp_get_wtime();
    printf("points 1 %f\n", single->left->point[0]);
    free_points(points, M[m]);
    
    printf("create single: %f\n", end-start);
    fprintf(output, "%d,%f", M[m], end-start);
                   
    printf("Points: %d.\n", M[m]);

    double times[3];
    for (int i = 0; i < 3; i++) {
      srand(SEED);
      points = gen_random_points(M[m], DIM, DEV);
      start = omp_get_wtime();
      Node* parallel = grow_tree_parallel(points, M[m], DIM, 0);
      end = omp_get_wtime();
      printf("points 2 %f\n", parallel->left->point[0]);
      free_points(points, M[m]);
      
      times[i] = end-start;
      printf("%d\n",compare_trees(single, parallel, DIM));
      assert(!compare_trees(single, parallel, DIM));
      free(parallel);
    }
    printf("create parallel: %f\n", (times[0]+times[1]+times[2])/3);
    fprintf(output, ",%f\n", (times[0]+times[1]+times[2])/3);
    free_tree(single);
  }
  fclose(output);
  
  
  
  printf("\n\nInsertion\n\n");
   
  output = fopen("insert.csv", "w");
  fprintf(output, "points_amount,serial,threaded\n");
  
  float** points = gen_random_points(MM_FOR_INSERT, DIM, DEV);
  Node* single = grow_tree(points, MM_FOR_INSERT, DIM, 0);
  free_points(points, MM_FOR_INSERT);

  double time = 0;
  float* point;
  for (int measure = 0; measure < MM_FOR_INSERT; measure++) {
    point = gen_random_point(DIM, DEV);
    start = omp_get_wtime();
    single = insert_point(single, point, DIM, 0);
    end = omp_get_wtime();
    time += end-start;
  }
  printf("insert single (x%d): %f\n", MM_FOR_INSERT, time);
  fprintf(output, "%d,%f", MM_FOR_INSERT, time);

  printf("Threads:\n");

  points = gen_random_points(MM_FOR_INSERT, DIM, DEV);
  Node* parallel = grow_tree(points, MM_FOR_INSERT, DIM, 0);
  free_points(points, MM_FOR_INSERT);

  time = 0;
  for (int measure = 0; measure < MM_FOR_INSERT; measure++) {
    point = gen_random_point(DIM, DEV);
    start = omp_get_wtime();
    parallel = insert_point_parallel(parallel, point, DIM, 0);
    end = omp_get_wtime();
    time += end-start;
  }
  printf("insert threads: %f\n", time);
  fprintf(output, "%d,%f", MM_FOR_INSERT, time);
  free_tree(single);
  free_tree(parallel);
  fclose(output);
  
  
  
  printf("\n\nSearch\n\n");
   
  output = fopen("search.csv", "w");
  fprintf(output, "points_amount,serial,threaded\n");
  
  points = gen_random_points(MM_FOR_FIND, DIM, DEV);
  single = grow_tree(points, MM_FOR_FIND, DIM, 0);

  time = 0;
  for (int measure = 0; measure < MM_FOR_FIND; measure++) {
    point = gen_random_point(DIM, DEV);
    start = omp_get_wtime();
    single = insert_point(single, point, DIM, 0);
    end = omp_get_wtime();
    time += end-start;
  }
  printf("search single (x%d): %f\n", MM_FOR_FIND, time);
  fprintf(output, "%d,%f", MM_FOR_FIND, time);
   
  return 0;
}
