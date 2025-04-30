#include <stdio.h>
#include <stdlib.h>

#include "points.h"
#include "kdtree.h"

int main() {
  int AMOUNT = 20;
  int DIM = 2;
  float DEVIATION = 4;
  
  printf("Sorting test\n");
  float** points = gen_random_points(AMOUNT, DIM, DEVIATION);
  print_points(points, AMOUNT, DIM);
  heapsort(points+1, 1, AMOUNT-1);
  printf("res\n");
  print_points(points, AMOUNT, DIM);
  free_points(points, AMOUNT);

  printf("Growing, inserting and distance test\n");
  points = gen_random_points(AMOUNT, DIM, DEVIATION);
  print_points(points, AMOUNT, DIM);
  Node* root = grow_tree(points, AMOUNT, DIM, 0);

  float point[2] = {2, 2};
  root = insert_point(root, (float*)point, DIM, 0);
  
  float point2[2] = {-2, 2};
  root = insert_point(root, (float*)point2, DIM, 0);

  float target[2] = {-1, 1};
  Node** best = malloc(sizeof(Node*));
  float best_dist = 32.0;
  closest_neighbor(root, DIM, (float*)target, 0, best, &best_dist);
  
  printf("Closest neighbor: %f, %f\n", best[0]->point[0], best[0]->point[1]);
    
  print_tree(root, DIM, 0);
  FILE* file = fopen("tree.txt", "w");
  save_tree(file, root, DIM);
  fclose(file);
  
  free_tree(root);
   
  return 0;
}
