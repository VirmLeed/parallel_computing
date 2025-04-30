#ifndef POINTS_H
#define POINTS_H

void swap_points(float** a, float** b);
void to_heap(float** points, int axis, int len, int i);
void heapsort(float** points, int axis, int len);
float** gen_random_points(int point_amount, int dimensions, float deviation);
float* gen_random_point(int dim, float deviation);
void free_points(float** points, int point_amount);
void print_points(float** points, int point_amount, int dimensions);
float distance(float* a, float* b, int dim);

#endif
