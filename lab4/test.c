#include <stdio.h>
#include <stdlib.h>

#include "points.h"
#include "kdtree.h"

int main() {
  int AMOUNT = 800000;
  int DIM = 2;
  float DEVIATION = 8;

  printf("Sorting test\n");
  float** points = gen_random_points(AMOUNT, DIM, DEVIATION);
  // print_points(points, AMOUNT, DIM);
  heapsort(points+1, 1, AMOUNT-1);
  printf("res\n");
  // print_points(points, AMOUNT, DIM);
  free_points(points, AMOUNT);

  printf("Growing, inserting and distance test\n");
  srand(1);
  points = gen_random_points(AMOUNT, DIM, DEVIATION);
  // print_points(points, AMOUNT, DIM);
  Node* root = grow_tree(points, AMOUNT, DIM, 0);
  free(points);

  srand(1);
  points = gen_random_points(AMOUNT, DIM, DEVIATION);
  // print_points(points, AMOUNT, DIM);
  Node* root2 = create_node(DIM);
  #pragma omp parallel
  {
    #pragma omp single
    {
      root2 = grow_tree_parallel(points, AMOUNT, DIM, 0);
    }
  }
  // Node* root2 = grow_tree_parallel(points, AMOUNT, DIM, 0);
  free_points(points, AMOUNT);
  printf("Comparison %d\n", compare_trees(root, root2, DIM));

  float point[2] = {2, 2};
  root = insert_point(root, (float*)point, DIM, 0);
  
  float point2[2] = {-2, 2};
  root = insert_point(root, (float*)point2, DIM, 0);

  float target[2] = {-1, 1};
  Node** best = malloc(sizeof(Node*));
  float best_dist = 32.0;
  closest_neighbor(root, DIM, (float*)target, 0, best, &best_dist);
  
  printf("Closest neighbor: %f, %f\n", best[0]->point[0], best[0]->point[1]);
    
  // print_tree(root, DIM, 0);
  FILE* file = fopen("tree.txt", "w");
  save_tree(file, root, DIM);
  fclose(file);
  
  free_tree(root);
   
  return 0;
}
