#include <stdio.h>
#include <stdlib.h>

#include "points.h"

void swap_points(float** a, float** b) {
  float* tmp = *a;
  *a = *b;
  *b = tmp;
}

void to_heap(float** points, int axis, int len, int i) {
  int largest = i;
  int left = 2*i + 1;
  int right = 2*i + 2;

  if (left < len && points[left][axis] > points[largest][axis])
    largest = left;

  if (right < len && points[right][axis] > points[largest][axis])
    largest = right;
  
  if (largest != i) {
    swap_points(&points[i], &points[largest]);
    to_heap(points, axis, len, largest);
  }
}

void heapsort(float** points, int axis, int len) {
  for (int i = len / 2 - 1; i >= 0; i--) {
    to_heap(points, axis, len, i);
  }
  
  for (int i = len - 1; i > 0; i--) {
    swap_points(points, &points[i]);
    to_heap(points, axis, i, 0);
  }
}

float** gen_random_points(int point_amount, int dimensions, float deviation) {
  float** points = (float**)malloc(point_amount * sizeof(float*));

  for (int i = 0; i < point_amount; i++) {
    points[i] = gen_random_point(dimensions, deviation);
  }

  return points;
}    

float* gen_random_point(int dim, float deviation) {
  float* point = malloc(dim * sizeof(float));
    
  for (int i = 0; i < dim; i++) {
    point[i] = ((float)rand() / RAND_MAX) * 2 * deviation - deviation;
  }

  return point;
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

float distance(float* a, float* b, int dim) {
  float result = 0;
  for (int axis = 0; axis < dim; axis++) {
    result += (a[axis] - b[axis]) * (a[axis] - b[axis]);
  }
    
  return result;
}