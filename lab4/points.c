#include <stdio.h>

void swap_points(float** a, float** b) {
  float* tmp = *a;
  *a = *b;
  *b = tmp;
}

void to_heap(float** points, int axis, int start, int end, int i) {
  int largest = i;
  int left = 2*i + 1;
  int right = 2*i + 2;

  if (left < (end - start) && points[start + left][axis] > points[start + largest][axis])
    largest = left;

  if (right < (end - start) && points[start + right][axis] > points[start + largest][axis])
    largest = right;
  
  if (largest != i) {
    printf("heap-swapping\n");
    swap_points(&points[start + i], &points[start + largest]);
    to_heap(points, axis, start, end, largest);
  }
}

void heapsort(float** points, int axis, int start, int end) {
  printf("heaping\n");
  for (int i = (end-start) / 2 - 1; i >= 0; i--) {
    to_heap(points, axis, start, end, i);
  }
  
  printf("swapping\n");
  for (int i = end - start - 1; i > 0; i--) {
    swap_points(&points[start], &points[start + i]);
    to_heap(points, axis, start, start + i, 0);
  }
}
