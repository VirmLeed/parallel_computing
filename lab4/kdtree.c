#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "kdtree.h"
#include "points.h"

Node* create_node(int k) {
  Node* output = malloc(sizeof(Node));
  output->point = malloc(sizeof(float) * k);

  return output;
}

Node* grow_tree(float** points, int point_amount, int dim, int depth) {
  if (point_amount <= 0) return NULL;  
   
  int axis = depth % dim;

  heapsort(points, axis, point_amount);

  int median = point_amount / 2;

  Node* node = create_node(dim);
  node->point = points[median];
  node->left = grow_tree(points, median, dim, depth + 1);
  node->right = grow_tree(points + median + 1, point_amount - median - 1, dim, depth + 1);

  return node;
}

Node* grow_tree_parallel(float** points, int point_amount, int dim, int depth) {
  if (point_amount <= 0) return NULL;  
  if (depth > 3) return grow_tree(points, point_amount, dim, depth);
   
  int axis = depth % dim;

  heapsort(points, axis, point_amount);

  int median = point_amount / 2;

  Node* node = create_node(dim);
  node->point = points[median];
  #pragma omp task
  node->left = grow_tree_parallel(points, median, dim, depth + 1);
  #pragma omp task
  node->right = grow_tree_parallel(points + median + 1, point_amount - median - 1, dim, depth + 1);

  #pragma omp taskwait
  return node;
}

void print_tree(Node* node, int dim, int depth) {
  if (node == NULL) return;
  
  printf("Depth: %d, Point: (", depth);
  for (int axis = 0; axis < dim; axis++)
    printf("%.2f, ", node->point[axis]);
  printf(")\n");
  
  print_tree(node->left, dim, depth+1);
  print_tree(node->right, dim, depth+1);
}

void save_tree(FILE* file, Node* node, int dim) {
  if (node == NULL) return;
  
  for (int axis = 0; axis < dim; axis++)
    fprintf(file, "%f ", node->point[axis]);
  fprintf(file, "\n");
  
  save_tree(file, node->left, dim);
  save_tree(file, node->right, dim);
}

void free_tree(Node* node) {
  if (node == NULL) return;
  
  free_tree(node->left);
  free_tree(node->right);
  free(node);
}

Node* insert_point(Node* root, float* point, int dim, int depth) {
    if (root == NULL) {
        Node* node = create_node(dim);
        node->point = point;
        node->left = node->right = NULL;
        return node;
    }

    int axis = depth % 2;

    if (point[axis] < root->point[axis]) {
        root->left = insert_point(root->left, point, dim, depth + 1);
    } else {
        root->right = insert_point(root->right, point, dim, depth + 1);
    }

    return root;
}

Node* insert_point_parallel(Node* root, float* point, int dim, int depth) {
  if (root == NULL) {
      Node* node = create_node(dim);
      node->point = point;
      node->left = node->right = NULL;
      return node;
  }
  if (depth > 3) return insert_point(root, point, dim, depth);

  int axis = depth % 2;

  if (point[axis] < root->point[axis]) {
    #pragma omp task
    root->left = insert_point_parallel(root->left, point, dim, depth + 1);
  } else {
    #pragma omp task
    root->right = insert_point_parallel(root->right, point, dim, depth + 1);
  }

  #pragma omp taskwait
  return root;
}

void closest_neighbor(Node* root, int dim, float* target, int depth, Node** best, float* best_dist) {
  if (root == NULL) return;

  float dist = distance(root->point, target, dim);

  if (dist < *best_dist) {
    *best_dist = dist;
    *best = root;
  }

  int axis = depth % 2;
  Node* next_branch = (target[axis] < root->point[axis]) ? root->left : root->right;
  Node* other_branch = (next_branch == root->left) ? root->right : root->left;

  closest_neighbor(next_branch, dim, target, depth + 1, best, best_dist);

  float axis_dist = (target[axis] - root->point[axis]) * (target[axis] - root->point[axis]);
  if (axis_dist < *best_dist) {
    closest_neighbor(other_branch, dim, target, depth + 1, best, best_dist);
  }
}

void closest_neighbor_parallel(Node* root, int dim, float* target, int depth, Node** best, float* best_dist) {
  if (root == NULL) return;

  float dist = distance(root->point, target, dim);

  if (dist < *best_dist) {
    *best_dist = dist;
    *best = root;
  }

  int axis = depth % 2;
  Node* next_branch = (target[axis] < root->point[axis]) ? root->left : root->right;
  Node* other_branch = (next_branch == root->left) ? root->right : root->left;

  #pragma omp task shared(best, best_dist)
  closest_neighbor(next_branch, dim, target, depth + 1, best, best_dist);

  float axis_dist = (target[axis] - root->point[axis]) * (target[axis] - root->point[axis]);
  if (axis_dist < *best_dist) {
    #pragma omp task shared(best, best_dist)
    closest_neighbor(other_branch, dim, target, depth + 1, best, best_dist);
  }
}