#include <stdlib.h>

typedef struct {
  float* point;
  struct Node* left;
  struct Node* right;
} Node;

Node* create_node(int k) {
  Node* output = malloc(sizeof(Node));
  output->point = malloc(sizeof(float) * k);

  return output;
}

Node* grow_tree(float** points, int point_amount, int depth, int k) {
  int axis = depth % k;

}
