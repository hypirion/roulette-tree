#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "rtree.h"

// Private functions, used inside the structure.

static void dfs_destroy(rtree_node_t *rnode) {
  if (rnode == NULL) {
    return;
  }
  dfs_destroy(rnode->link[0]);
  dfs_destroy(rnode->link[1]);
  free(rnode);
}

static bool is_red(rtree_node_t *rt) {
  return rt != NULL && rt->red == true;
}

static rtree_node_t *rot_once(rtree_node_t *root, int dir) {
  rtree_node_t *ret = root->link[!dir];

  // TODO: fitness must be rotated properly too.

  root->link[!dir] = ret->link[dir];
  ret->link[dir] = root;

  root->red = true;
  ret->red = false;

  return ret;
}

static rtree_node_t *rot_twice(rtree_node_t *root, int dir) {
  root->link[!dir] = rot_once(root->link[!dir], !dir);
  return rot_once(root, dir);
}

static rtree_node_t *rtree_node_create(void *data, double fitness) {
  rtree_node_t *node = (rtree_node_t *) malloc(sizeof(rtree_node_t));
  node->data = data;
  node->fit = fitness;
  node->tot = fitness;
  node->link_sum[0] = 0;
  node->link_sum[1] = 0;
  node->red = true;
  node->link[0] = NULL;
  node->link[1] = NULL;
  return node;
}

static rtree_node_t *rtree_insert_node(rtree_node_t *root, void *data,
                                       double fitness) {
  if (root == NULL) {
    return rtree_node_create(data, fitness);
  }
  else {
    int dir = 0; // TODO: Tactical node insertions in the making.

    root->link[dir] = rtree_insert_node(root->link[dir], data, fitness);
    root->link_sum[dir] += fitness;
    root->tot += fitness;

    // Rebalancing
    if (is_red(root->link[dir])) {
        if (is_red(root->link[!dir])) { // Flip
            root->red = true;
            root->link[0]->red = false;
            root->link[1]->red = false;
          }
        else { // Rotate.
          if (is_red(root->link[dir]->link[dir])) {
              root = rot_once(root, !dir);
            }
          else if (is_red(root->link[dir]->link[!dir])) {
            root = rot_twice(root, !dir);
          }
        }
      }

    return root;
  }
}

// Public functions, available for everyone to use.

rtree_t *rtree_create() {
  rtree_t *rt = malloc(sizeof(rtree_t));
  rt->root = NULL;
  return rt;
}

void rtree_destroy(rtree_t *rt) {
  dfs_destroy(rt->root);
  free(rt);
}

void rtree_add(rtree_t *rt, void *data_ptr, double fitness) {
  rt->root = rtree_insert_node(rt->root, data_ptr, fitness);
  rt->root->red =false;
}

void *rtree_rget(rtree_t *rt) {
  rtree_node_t *cur = rt->root;
  double pick = drand48() * cur->tot;
  while (1) {
    if (cur->link_sum[0] < pick) { // this or right
      pick -= cur->link_sum[0];
      if (pick < cur->fit) { // this
        return cur->data;
      }
      else { // right
        pick -= cur->fit;
        cur = cur->link[1];
      }
    }
    else { // left
      cur = cur->link[0];
    }
  }
}
