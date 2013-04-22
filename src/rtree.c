#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "rtree.h"

rtree_t *rtree_create() {
  rtree_t *rt = malloc(sizeof(rtree_t));
  rt->root = NULL;
  return rt;
}

static void dfs_destroy(rtree_node_t *rnode) {
  if (rnode == NULL) {
    return;
  }
  dfs_destroy(rnode->link[0]);
  dfs_destroy(rnode->link[1]);
  free(rnode);
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
