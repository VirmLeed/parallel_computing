#include <stdio.h>
#include <stdlib.h>

#include "points.h"

float** gen_random_points(int point_amount, int dimensions, float deviation) {
  float** points = (float**)malloc(point_amount * sizeof(float*));

  for (int i = 0; i < point_amount; i++) {
    
    points[i] = (float*)malloc(dimensions * sizeof(float));
      
    for (int j = 0; j < dimensions; j++) {
      points[i][j] = ((float)rand() / RAND_MAX) * 2 * deviation - deviation;
    }
  }

  return points;
}    

void free_points(float** points, int point_amount) {
    for (int i = 0; i < point_amount; i++) {
        free(points[i]);
    }
    free(points);
}

void print_points(float** points, int point_amount, int dimensions) {
  for (int i = 0; i < point_amount; i++) {
    for (int j = 0; j < dimensions; j++) {
      printf("%.2f ", points[i][j]);
    }
    printf("\n");
  }
}

int main() {
  int AMOUNT = 5;
  int DIM = 3;
  float** points = gen_random_points(AMOUNT, DIM, 10);
  
  print_points(points, AMOUNT, DIM);
  heapsort(points, 1, 1, AMOUNT-1);
  print_points(points, AMOUNT, DIM);


  free_points(points, AMOUNT);
  return 0;
}
