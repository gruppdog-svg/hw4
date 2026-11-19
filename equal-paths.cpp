#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

static int equalDepth(Node* node){
  if(node == nullptr){
    return 0;
  }

  if (node->left == nullptr && node->right == nullptr){
    return 1;
  }

  if (node->left == nullptr){
    int r = equalDepth(node->right);
     return (r == -1) ? -1 : r + 1;
  }

  if (node->right == nullptr){
    int l = equalDepth(node->left);
     return (l == -1) ? -1 : l + 1;
  }

  int l = equalDepth(node->left);
  int r = equalDepth(node->right);

  if(l == -1 || r == -1 || l != r){
    return -1;
  }

  return l + 1;
}


bool equalPaths(Node * root)
{
    // Add your code below
    return equalDepth(root) != -1;

}

