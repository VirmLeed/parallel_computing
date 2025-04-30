#ifndef KDTREE_H
#define KDTREE_H

#include <stdio.h>

typedef struct Node {
  float* point;
  struct Node* left;
  struct Node* right;
} Node;

Node* create_node(int dim);
Node* grow_tree(float** points, int point_amount, int dim, int depth);
Node* grow_tree_parallel(float** points, int point_amount, int dim, int depth);
void print_tree(Node* node, int dim, int depth);
void free_tree(Node* node);
void save_tree(FILE* file, Node* root, int dim);
Node* insert_point(Node* root, float* point, int dim, int depth);
Node* insert_point_parallel(Node* root, float* point, int dim, int depth);
void closest_neighbor(Node* root, int dim, float* target, int depth, Node** best, float* best_dist);
void closest_neighbor(Node* root, int dim, float* target, int depth, Node** best, float* best_dist);

#endif
