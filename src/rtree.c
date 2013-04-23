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

void *rtree_rget(rtree_t *rt) {
  rtree_node_t *cur = rt->root;
  double pick = drand48() * cur->tot;
  while (1) {
    if (cur->lsum < pick) { // this or right
      pick -= cur->lsum;
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
