#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#include "points.h"
#include "kdtree.h"

#define TT 4
#define MM 4
#define MM_FOR_INSERT 1000000
#define MM_FOR_FIND 1000000

int main() {
  omp_set_nested(1);
  omp_set_num_threads(4);
    
  
  // const int T[TT] = {1, 2, 3, 4};//, 5, 6, 7, 8};
  const int M[MM] = {100000, 200000, 400000, 800000};

  int DIM = 2;
  float DEV = 8;
  int SEED = 2;

  double start,end;
  FILE* output;
  
  printf("\n\nCreation\n\n");
  output = fopen("create.csv", "w");
  fprintf(output, "points_amount,serial,threaded\n");
  
  // for (int t = 0; t < TT; t++) {
  // omp_set_num_threads(T[t]);
    for (int m = 0; m < MM; m++) {
      printf("Points: %d.\n", M[m]);
                   
      float** points_single;
    
      srand(SEED);
      points_single = gen_random_points(M[m], DIM, DEV);
      start = omp_get_wtime();
      Node* single = grow_tree(points_single, M[m], DIM, 0);
      end = omp_get_wtime();
      // printf("points 1 %f\n", single->left->point[0]);
    
      printf("create single: %f\n", end-start);
      fprintf(output, "%d,%f", M[m], end-start);

      double times[3];
      float** points_parallel;
      for (int i = 0; i < 3; i++) {
        srand(SEED);
        points_parallel = gen_random_points(M[m], DIM, DEV);
        start = omp_get_wtime();
        Node* parallel = create_node(DIM);
        // #pragma omp parallel
        // {
          // #pragma omp single
          // {
            parallel = grow_tree_parallel(points_parallel, M[m], DIM, 0);
          // }
        // }
        end = omp_get_wtime();
        // printf("points 2 %f\n", parallel->left->point[0]);
      
        times[i] = end-start;
        // printf("%d\n",compare_trees(single, parallel, DIM));
        assert(!compare_trees(single, parallel, DIM));
        free_tree(parallel);
      }

      free_points(points_single, M[m]);
      free_points(points_parallel, M[m]);

      printf("create parallel: %f\n", (times[0]+times[1]+times[2])/3);
      fprintf(output, ",%f\n", (times[0]+times[1]+times[2])/3);
      free_tree(single);
    }
  // }
  fclose(output);
  
  
  
  printf("\n\nInsertion\n\n");
   
  output = fopen("insert.csv", "w");
  fprintf(output, "points_amount,serial,threaded\n");

  srand(SEED);
  float** points_single = gen_random_points(MM_FOR_INSERT, DIM, DEV);
  Node* single = grow_tree(points_single, MM_FOR_INSERT, DIM, 0);

  double time = 0;
  float* point;
  srand(SEED);
  for (int measure = 0; measure < MM_FOR_INSERT; measure++) {
    point = gen_random_point(DIM, DEV);
    start = omp_get_wtime();
    single = insert_point(single, point, DIM, 0);
    end = omp_get_wtime();
    time += end-start;
  }
  printf("insert single (x%d): %f\n", MM_FOR_INSERT, time);
  fprintf(output, "%d,%f", MM_FOR_INSERT, time);

  srand(SEED);
  float** points_parallel = gen_random_points(MM_FOR_INSERT, DIM, DEV);
  Node* parallel = grow_tree(points_parallel, MM_FOR_INSERT, DIM, 0);

  time = 0;
  srand(SEED);
  for (int measure = 0; measure < MM_FOR_INSERT; measure++) {
    point = gen_random_point(DIM, DEV);
    start = omp_get_wtime();
    #pragma omp parallel
    {
      #pragma omp single
      {
        // parallel = insert_point(parallel, point, DIM, 0);
        parallel = insert_point_parallel(parallel, point, DIM, 0);
      }
    }
    end = omp_get_wtime();
    time += end-start;
  }

  printf("insert threads: %f\n", time);
  fprintf(output, "%d,%f", MM_FOR_INSERT, time);
  assert(!compare_trees(single, parallel, DIM));
  free_points(points_single, MM_FOR_INSERT);
  free_points(points_parallel, MM_FOR_INSERT);
  free_tree(single);
  free_tree(parallel);
  free(point);
  fclose(output);
  
  
  
  printf("\n\nSearch\n\n");
   
  output = fopen("search.csv", "w");
  fprintf(output, "points_amount,serial,threaded\n");
  
  srand(SEED);
  points_single = gen_random_points(MM_FOR_FIND, DIM, DEV);
  single = grow_tree(points_single, MM_FOR_FIND, DIM, 0);

  time = 0;
  float best_dist = 9999999;
  Node** best = &single;
  srand(SEED);
  for (int measure = 0; measure < MM_FOR_FIND; measure++) {
    point = gen_random_point(DIM, DEV);
    start = omp_get_wtime();
    closest_neighbor(single, DIM, point, 0, best, &best_dist);
    end = omp_get_wtime();
    time += end-start;
  }
  printf("search single (x%d): %f\n", MM_FOR_FIND, time);
  fprintf(output, "%d,%f", MM_FOR_FIND, time);

  srand(SEED);
  points_parallel = gen_random_points(MM_FOR_FIND, DIM, DEV);
  parallel = grow_tree(points_single, MM_FOR_FIND, DIM, 0);

  time = 0;
  srand(SEED);
  best_dist = 9999999;
  best = &parallel;
  for (int measure = 0; measure < MM_FOR_FIND; measure++) {
    point = gen_random_point(DIM, DEV);
    start = omp_get_wtime();
    #pragma omp parallel
    {
      #pragma omp single
      {
        // closest_neighbor(parallel, DIM, point, 0, best, &best_dist);
        closest_neighbor_parallel(parallel, DIM, point, 0, best, &best_dist);
      }
    }
    end = omp_get_wtime();
    time += end-start;
  }
  printf("search threads (x%d): %f\n", MM_FOR_FIND, time);
  fprintf(output, "%d,%f", MM_FOR_FIND, time);

  free_points(points_single, MM_FOR_FIND);
  free_points(points_parallel, MM_FOR_FIND);
  free_tree(single);
  free_tree(parallel);

  return 0;
}
