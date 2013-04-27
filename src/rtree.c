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

  root->link_sum[!dir] = ret->link_sum[dir];
  root->tot = root->fit + root->link_sum[0] + root->link_sum[1];

  root->link[!dir] = ret->link[dir];
  ret->link[dir] = root;

  ret->link_sum[dir] = root->tot;
  ret->tot = ret->fit + ret->link_sum[0] + ret->link_sum[1];

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
    rtree_node_t head = {0};

    rtree_node_t *g, *t, *p, *q;
    int dir = 0, last = 2;
    bool inserted = false;

    t = &head;
    g = p = NULL;
    q = t->link[1] = root;

    while (true) {
      if (q == NULL) {
        p->link[dir] = q = rtree_node_create(data, fitness);
        inserted = true;
      }
      else if (is_red(q->link[0]) && is_red(q->link[1])) {
        q->red = true;
        q->link[0]->red = false;
        q->link[1]->red = false;
      }

      if (!inserted) {
        q->link_sum[dir] += fitness;
        q->tot += fitness;
      }

      if (is_red(q) && is_red(p)) {
        int d2 = t->link[1] == g;

        if (q == p->link[last]){
          t->link[d2] = rot_once(g, !last);
        }
        else {
          t->link[d2] = rot_twice(g, !last);
        }
      }

      if (inserted) {
        break;
      }

      last = dir;
      dir = 0; // Magic work here (later on)

      if (g != NULL) {
        t = g;
      }
      g = p, p = q;
      q = q->link[dir];
    }

    return head.link[1];
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
  rt->root->red = false;
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

#ifdef RTREE_DEBUG

#include <stdio.h>

int rb_check(rtree_node_t *root) {
  int h[2];

  if (root == NULL) {
    return 1;
  }
  else {
    rtree_node_t **node = root->link;

    /* multiple red links */
    if (is_red(root)) {
      if (is_red(node[0]) || is_red(node[1])) {
        puts("Red violation");
        return 0;
      }
    }

    /* check links */
    h[0] = rb_check(node[0]);
    h[1] = rb_check(node[1]);

    /* black height mismatch */
    if (h[0] > 0 && h[1] > 0 && h[0] != h[1]) {
      puts("Black violation");
      return 0;
    }

    if (h[0] > 0 && h[1] > 0) {
      return is_red(root) ? h[0] : h[0] + 1;
    }
    else {
      return 0;
    }
  }
}

#define DELTA 0.001
#include <math.h>

double total_fit_check(rtree_t *rt) {
  double calc_fit = fit_check(rt->root);
  if (fabs(calc_fit - rt->root->tot) < DELTA) {
    printf("root->fit adds up (%f)\n", rt->root->tot);
    return calc_fit;
  }
  else {
    puts("root->fit doesn't add up with total sum.");
    printf("Expected %f, was %f.\n", calc_fit, rt->root->tot);
    return -1;
  }
}

double fit_check(rtree_node_t *root) {
  if (root == NULL) {
    return 0;
  }
  double calc_tot = root->fit;
  for (int i = 0; i < 2; i++) {
    double actual = root->link_sum[i];
    double calculated = fit_check(root->link[i]);
    calc_tot += calculated;
    if (calculated == -1) {
      return -1;
    }
    if (fabs(actual - calculated) >= DELTA) {
      puts("Sum doesn't add up.");
      printf("Expected %f, was %f.\n", calculated, actual);
      return -1;
    }
  }
  if (fabs(calc_tot - root->tot) < DELTA) {
    return calc_tot;
  }
  else {
    return -1;
  }
}

static void rtree_node_preorder(rtree_node_t *root, int tab) {
  printf("%*s", tab, "");
  if (root == NULL) {
    printf("NULL\n");
  }
  else {
    printf("{%d, [%.2f] + %.2f + [%.2f] -> %.2f}\n", root->red,
           root->link_sum[0], root->fit, root->link_sum[1], root->tot);
    for (int i = 0; i < 2; i++) {
      rtree_node_preorder(root->link[i], tab + 2);
    }
  }
}

void rtree_preorder(rtree_t *rt) {
  rtree_node_preorder(rt->root, 0);
}

#endif
