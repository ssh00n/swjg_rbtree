#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>


rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *nilNode = (node_t *)calloc(1, sizeof(node_t));     // calloc() 함수는 malloc과 달리 각 요소의 모든 비트에 초기 값 0을 제공(0으로 초기화)
  
  nilNode->color = RBTREE_BLACK;
  p->nil = nilNode;
  p->root = nilNode;
  
  return p;
}

void right_rotate(rbtree *t, node_t *y) {
  node_t *x; 

  x = y->left;                 // y애 대하여 right rotate를 수행했을 때 y의 자리에 들어올 노드(y의 left) 
  y->left = x->right;          // rotate 되면서 x의 right child가 y가 되므로, 기존에 있던 x의 right는 y의 left로 붙음

  if (x->right != t->nil)  {         // x의 right가 nil이 아니면 
    x->right->parent = y;            // x의 right의 parent를 y로 지정
  }
  x->parent = y->parent;              // rotate되어 y의 자리에 x가 들어오므로 x의 parent에 y의 parent를 넣어줌
  
  if (y->parent == t->nil)              // y가 root일 떄
    t->root = x;                        
  else if (y == y->parent->left) {              // y가 left child
    y->parent->left = x;   
  }
  else {                                        // y가 right child
    y->parent->right = x;
  }
  
  x->right = y;                                 //rotate를 마치고 x의 right를 y로
  y->parent = x;                                // y의 parent를 x로 바꿈
}

void left_rotate(rbtree *t, node_t *y) {
  node_t *x;

  x = y->right;               // rotate한 자리에 들어올 x 지정 (y의 right)
  y->right = x->left;         // rotate 되면서 x의 left chid가 y가 되므로, 기존에 있던 x의 left는 y의 right로 붙음
  
  if (x->left != t->nil) {           // x의 left가 nil이 아니면
    x->left->parent = y;             // parent를 y로 지정
  }
  x->parent = y->parent;             // rotate되어 y의 자리에 x가 들어오므로 x의 parent에 y의 parent를 넣어줌

  if (y->parent == t->nil) {            // y가 root일 때
    t->root = x;
  }
  else if (y == y->parent->left) {      // y가 left child
    y->parent->left = x;
  }
  else {                                // y가 right child
    y->parent->right=x;
  }
  x->left = y;                          // rotate를 마치고 x의 left를 y로
  y->parent = x;                        // y의 parent를 x로 바꿈
  
}

void free_node(rbtree *t, node_t *x) {       // 후위 순회 방식으로 노드와 하위 노드들의 메모리를 반환하는 함수
  if (x->left != t->nil)
    free_node(t, x->left);
  if (x->right != t->nil)
    free_node(t, x->right);

  free(x);
  x = NULL;                                  // 반환 후 포인터를 NULL로 초기화
}

void delete_rbtree(rbtree *t) {             // RB 트리의 메모리를 반환하는 함수
  if (t->root != t->nil)
    free_node(t, t->root);
  free(t->nil);
  free(t);
}

void rbtree_insert_fixup(rbtree *t, node_t *z) {      // RB 트리에 노드를 삽입하고 RB properties를 위반했을 때 해결하는 함수      
  node_t *y; 

  while (z->parent->color == RBTREE_RED){
    if (z->parent == z->parent->parent->left) {         // parent of z == left subtree
      y = z->parent->parent->right;                     // y : uncle
      if (y->color == RBTREE_RED) {                    // Case 1 : uncle.color == RED, parent.color == RED      
        z->parent->color = RBTREE_BLACK;               // parent, uncle의 color는 black으로, grandparent는 red로
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;                        // z를 위로 옮겨주고 다시 parent가 red인지 확인
      }
      else {                                         //  Case 2 : uncle.color == BLACK
        if (z == z->parent->right) {                 // 꺾여있음 -> LEFT ROTATE 하여 Case 3으로 만들어 줌
        z = z->parent;
        left_rotate(t, z);       
        }                                            // Case 3 : parent = red, left child, uncle.color = BLACK
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t, z->parent->parent);
      }
    }

    else {  // right & left exchanged (z->parent == z->parent->parent->left)
      y = z->parent->parent->left;
      if (y->color == RBTREE_RED) {  
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;                           
      }
      else {
        if (z == z->parent->left) {                       
            z = z->parent;
            right_rotate(t, z);
          }
          z->parent->color = RBTREE_BLACK;
          z->parent->parent->color = RBTREE_RED;
          left_rotate(t, z->parent->parent);
          }
        }
      }
    t->root->color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {

  node_t* x;
  node_t* y;
  node_t* z = (node_t*)calloc(1, sizeof(node_t));
  z->key = key;
  x = t->root;                  // node being compared with z
  y = t->nil;                   // y will be parent of z
  
  while (x != t->nil) {         // descend until reaching the sentinel
    y = x;
    if (z->key < x->key) {
      x = x->left;
    }
    else {
      x = x->right;
    }
  }

  z->parent = y;                 // found the location to be inserted -> insert z with parent y

  if(y == t->nil) {
    t->root = z;
  }
  else if (z->key < y->key) {
    y->left = z;
  }
  else {
    y->right = z;
  }
  z->left = t->nil;
  z->right = t->nil;
  z->color = RBTREE_RED;
  
  rbtree_insert_fixup(t, z);
  return t->root;
}


node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *x;

  x = t->root;
  while (x != t->nil) {
    if (x->key == key) {
      return x;
    }
    else if (x->key < key) {
      x = x->right;
    }
    else if (x->key > key){
      x = x->left;
    }
  }
  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  node_t *x;
  if (t->root == t->nil)
    return NULL;
   x = t->root;
   while (x->left != t->nil) {
     x = x->left;
   }
   return x;
}

node_t *rbtree_max(const rbtree *t) {
  node_t *x;

  if (t->root == t->nil)
    return NULL;
  x = t->root;
  while (x->right != t->nil) {
    x = x->right;
  }
  return x;
}

void rbtree_transplant(rbtree *t, node_t *u, node_t *v){     
  if (u->parent == t->nil)       // 삭제되는 노드가 root 일 경우
    t->root = v;
  else if (u == u->parent->left) // 삭제되는 노드가 왼쪽 노드일 경우
    u->parent->left = v;
  else                           // 삭제되는 노드가 오른쪽 노드일 경우
    u->parent->right = v;
  v->parent = u->parent;
}

void rbtree_delete_fixup(rbtree *t, node_t *x) {
  while (x != t->root && x->color == RBTREE_BLACK){
    // case 1 ~ 4
    if (x == x->parent->left)  {              // is x a left child?
      node_t *w = x->parent->right;           // w : uncle

      if (w->color == RBTREE_RED){                   // case 1
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        w = x->parent->right;
      }

      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK){ // case 2
        w->color = RBTREE_RED;
        x = x->parent;
      }

      else{
        if (w->right->color == RBTREE_BLACK){  // case 3
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          right_rotate(t, w);
          w = x->parent->right;
        }

        w->color = x->parent->color;              // case 4
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t->root;
        }
    }

    else {  // 반대 방향
      node_t *w = x->parent->left;

      if (w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }
      
      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;
        x = x->parent;
      }

      else
      {
        if (w->left->color == RBTREE_BLACK){
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t, w);
          w = x->parent->left;
        }
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }
x->color = RBTREE_BLACK;
}

int rbtree_erase(rbtree *t, node_t *p) {
  node_t *y; 
  node_t *x;
  color_t y_original_color;

  y = p;
  y_original_color = y->color;
  if (p->left == t->nil) { // 삭제될 노드의 left가 nil이면 -> right를 그대로 가져옴
    x = p->right;
    rbtree_transplant(t, p, p->right);
  }
  else if (p->right == t->nil) {    // 삭제될 노드의 right가 nil -> left를 그대로 가져옴
    x = p->left;
    rbtree_transplant(t, p, p->left);
  }
  else {                // 노드 p의 left, right child가 모두 nil이 아닌 경우
    y = p->right;
    while(y->left != t->nil){
      y = y->left;                     // find successor y
      }
    y_original_color = y->color;
    x = y->right;   // successor의 오른쪽 자식
    if (y->parent == p){
      x->parent = y;
    }
    else {                                   // 삭제된 p의 위치에 들어올 successor가 멀리 떨어져 있음
      rbtree_transplant(t, y, y->right);     // 올라오기 전에 successor의 위치를 successor의 right로 대체
      y->right = p->right;                   // 삭제될 p의 right들을 y의 right에 붙여줌
      y->right->parent = y;                 //  y의 자녀들(p에서 옮겨진 자녀들)의 parent를 y로 수정
      }

    rbtree_transplant(t, p, y);            // p 를 successor y로 대체
    y->left = p->left;             // 삭제될 p의 left들을 y의 left에 붙여 줌
    y->left->parent = y;           // 붙인 p의 자녀들의 parent를 y로 수정     
    y->color = p->color;            // 삭제된 p의 color를 y가 가짐 
  }
  if (y_original_color == RBTREE_BLACK)   {  // 삭제된 자리를 채운 y의 원래 색깔이 BLACK일 경우
    rbtree_delete_fixup(t, x);
  }
  free(p);
  return 0; 
}


void inorderTraversal(const rbtree *t, node_t* x, int* arr, int* idx, const size_t n){
  if (x == t->nil)
      return;

  // if (x->key !=0){
    inorderTraversal(t, x->left, arr, idx, n);
    arr[*idx] = x->key;
    if (*idx == n-1)
      return;
    (*idx)++;
    inorderTraversal(t, x->right, arr, idx, n);
  // }
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  int idx = 0;

  // array에 key 순서대로 삽입하기 위해 중위 순회를 진행 

  inorderTraversal(t, t->root, arr, &idx, n);
  return *arr;

}
