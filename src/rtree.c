/*
 * Copyright (c) 2013 Jean Niklas L'orange. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "rtree.h"

// Private functions, used inside the structure.

static void dfs_destroy(RTreeNode *rnode) {
  if (rnode == NULL) {
    return;
  }
  dfs_destroy(rnode->link[0]);
  dfs_destroy(rnode->link[1]);
  free(rnode);
}

static bool is_red(RTreeNode *rt) {
  return rt != NULL && rt->red == true;
}

static RTreeNode *rot_once(RTreeNode *root, int dir) {
  RTreeNode *ret = root->link[!dir];

  root->link_sum[!dir] = ret->link_sum[dir];
  root->tot = root->fit + root->link_sum[0] + root->link_sum[1];

  root->len[!dir] = ret->len[dir];
  ret->len[dir] += root->len[dir] + 1;

  root->link[!dir] = ret->link[dir];
  ret->link[dir] = root;

  ret->link_sum[dir] = root->tot;
  ret->tot = ret->fit + ret->link_sum[0] + ret->link_sum[1];

  root->red = true;
  ret->red = false;

  return ret;
}

static RTreeNode *rot_twice(RTreeNode *root, int dir) {
  root->link[!dir] = rot_once(root->link[!dir], !dir);
  return rot_once(root, dir);
}

static RTreeNode *rtree_node_create(void *data, double fitness) {
  RTreeNode *node = (RTreeNode *) malloc(sizeof(RTreeNode));
  node->data = data;
  node->fit = fitness;
  node->tot = fitness;
  node->len[0] = 0;
  node->len[1] = 0;
  node->link_sum[0] = 0;
  node->link_sum[1] = 0;
  node->red = true;
  node->link[0] = NULL;
  node->link[1] = NULL;
  return node;
}

static RTreeNode *rtree_insert_node(RTreeNode *root, void *data,
                                       double fitness) {
  if (root == NULL) {
    return rtree_node_create(data, fitness);
  }
  else {
    RTreeNode head = {0};

    RTreeNode *g, *t, *p, *q;
    int last = 2;
    bool inserted = false;

    t = &head;
    g = p = NULL;
    q = t->link[1] = root;
    int dir = q->len[0] > q->len[1];

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
      dir = q->len[0] > q->len[1];

      q->link_sum[dir] += fitness;
      q->len[dir]++;
      q->tot += fitness;

      if (g != NULL) {
        t = g;
      }
      g = p, p = q;
      q = q->link[dir];
    }

    return head.link[1];
  }
}

static double rtree_find_fit(RTree *rt, double pick) {
  RTreeNode *cur = rt->root;
  while (1) {
    if (cur->link_sum[0] < pick) { // this or right
      pick -= cur->link_sum[0];
      if (pick < cur->fit) { // this
        return cur->fit;
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

// Public functions, available for everyone to use.

RTree *rtree_create() {
  RTree *rt = malloc(sizeof(RTree));
  rt->root = NULL;
  return rt;
}

void rtree_destroy(RTree *rt) {
  dfs_destroy(rt->root);
  free(rt);
}

void rtree_add(RTree *rt, void *data_ptr, double fitness) {
  rt->root = rtree_insert_node(rt->root, data_ptr, fitness);
  rt->root->red = false;
}

void *rtree_rget(RTree *rt) {
  RTreeNode *cur = rt->root;
  double pick = ((double) rand()/(double) RAND_MAX) * cur->tot;
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

void *rtree_rpop(RTree *rt) {
  if (rt->root != NULL) {
    void *data_ptr = NULL;
    RTreeNode *root = rt->root;

    /* Pick a random element */
    const double pick = ((double) rand()/(double) RAND_MAX) * root->tot;
    const double fitness = rtree_find_fit(rt, pick);
    double fit_left = pick;

    /* Helpers */
    RTreeNode head = {0};
    RTreeNode *cur, *parent, *grandparent;

    cur = &head;
    parent = grandparent = NULL;
    cur->link[1] = root;

    int dir = 1;
    bool found = false;
    int prev_dir = 0;

    while (cur->link[dir] != NULL) {
      prev_dir = dir;

      /* Update helpers */
      grandparent = parent, parent = cur;
      cur = cur->link[dir];

      /* pick direction here */
      if (!found) {
        /* If the element is not in the left child tree. */
        if (cur->link_sum[0] < fit_left) {
          fit_left -= cur->link_sum[0];
          dir = 1;
          if (fit_left < cur->fit) {
            found = true;
            data_ptr = cur->data;
            /* Will modify dir when bubbling up values. */
          } else {
            fit_left -= cur->fit;
          }
        }
        else { /* If the element is in the left child tree.*/
          dir = 0;
        }
      }

      if (!found) {
        cur->tot -= fitness;
        cur->link_sum[dir] -= fitness;
      }
      else if (found) {
        /* Pick the largest tree and walk that one. */
        dir = cur->link_sum[0] < cur->link_sum[1];

        /* Bubble up value and fitness from child. */
        if (cur->link[dir] != NULL) {
          const RTreeNode *child = cur->link[dir];

          cur->tot -= cur->fit;
          cur->fit = child->fit;
          cur->link_sum[dir] -= child->fit;
          cur->data = child->data;
        }
      }

      cur->len[dir]--;

      /* Push red nodes downwards */
      if (!is_red(cur) && !is_red(cur->link[dir])) {
        if (is_red(cur->link[!dir])) {
          /* next element (black) we'll go down has a red sibling */
          /* Rotate once in direction we're walking. */
          parent = parent->link[prev_dir] = rot_once(cur, dir);
        }
        else if (!is_red(cur->link[!dir])) {
          RTreeNode *cur_sibling = parent->link[!prev_dir];

          if (cur_sibling != NULL) {
            /* If both sibling's children are black */
            if (!is_red(cur_sibling->link[0]) &&
                !is_red(cur_sibling->link[1])) {
              /* Bubble down redness */
              parent->red = false;
              cur_sibling->red = true;
              cur->red = true;
            }
            else {
              int gramp_dir = grandparent->link[1] == parent;

              if (is_red(cur_sibling->link[prev_dir])) {
                grandparent->link[gramp_dir] = rot_twice(parent, prev_dir);
              }
              else if (is_red(cur_sibling->link[!prev_dir])) {
                grandparent->link[gramp_dir] = rot_once(parent, prev_dir);
              }

              /* Make sure that the colours are correct: New child of
                 grandparent and current node should be red, whereas the
                 children of the new child of grandparent should be black
                 (obviously) */
              cur->red = grandparent->link[gramp_dir]->red = true;
              grandparent->link[gramp_dir]->link[0]->red = false;
              grandparent->link[gramp_dir]->link[1]->red = false;
            }
          }
        }
      }
    }

    /* Slaughter the last node here */
    parent->link[prev_dir] = NULL;
    free(cur);

    /* Update root and make it black */
    rt->root = head.link[1];
    if (rt->root != NULL) {
      rt->root->red = false;
    }
    return data_ptr;
  } else { // if (rt->root == NULL)
    return NULL;
  }
}

uint32_t rtree_size(RTree *rt) {
  if (rt->root == NULL) {
    return 0;
  } else {
    return rt->root->len[0] + 1 + rt->root->len[1];
  }
}

double rtree_total_fitness(RTree *rt) {
  if (rt->root == NULL) {
    return 0;
  } else {
    return rt->root->tot;
  }
}

#ifdef RTREE_DEBUG

#include <stdio.h>

int rb_check(RTreeNode *root) {
  int h[2];

  if (root == NULL) {
    return 1;
  }
  else {
    RTreeNode **node = root->link;

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

double total_fit_check(RTree *rt) {
  if (rt->root == NULL) {
    return 0.0;
  }
  double calc_fit = fit_check(rt->root);
  if (fabs(calc_fit - rt->root->tot) < DELTA) {
    // printf("root->fit adds up (%f)\n", rt->root->tot);
    return calc_fit;
  }
  else {
    puts("root->fit doesn't add up with total sum.");
    printf("Expected %f, was %f.\n", calc_fit, rt->root->tot);
    return -1;
  }
}

double fit_check(RTreeNode *root) {
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

static void rtree_node_preorder(RTreeNode *root, int tab) {
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

void rtree_preorder(RTree *rt) {
  rtree_node_preorder(rt->root, 0);
}

static int rtree_to_dot_rec(FILE *const out, RTreeNode *const root,
                            const int counter) {
  if (root == NULL)
    return counter;
  else {
    int link_names[2] = {0};
    int prev_link = counter;
    for (int i = 0; i < 2; i++) {
      prev_link =
        link_names[i] = rtree_to_dot_rec(out, root->link[i], prev_link);
    }
    const int mid = link_names[2-1] + 1;
    // print data node
    fprintf(out,
            "s%.3d [label=<\n"
            "<table border=\"0\" cellborder=\"1\" cellspacing=\"0\" "
            "cellpadding=\"6\" align=\"center\">\n"
            "  <tr>\n"
            "    <td height=\"36\" width=\"25\" port=\"0\">%ld</td>\n"
            "  </tr>\n"
            "</table>>];\n",
            mid, (long) root->data);
    const int this = mid + 1;
    fprintf(out,
            "  s%.3d [label=<\n"
             "<table border=\"0\" cellborder=\"1\" cellspacing=\"0\" "
            "cellpadding=\"6\" align=\"center\" bgcolor=\"#%s\">\n"
            "  <tr>\n"
            "    <td height=\"36\" width=\"25\">%d</td>\n"
            "    <td height=\"36\" width=\"25\">%s</td>\n"
            "    <td height=\"36\" width=\"25\">%d</td>\n"
            "  </tr>\n"
            "  <tr>\n"
            "    <td colspan=\"3\" height=\"36\" width=\"25\">%.2f</td>\n"
            "  </tr>\n"
            "  <tr>\n"
            "    <td height=\"36\" width=\"25\" port=\"0\">%.2f</td>\n"
            "    <td height=\"36\" width=\"25\" port=\"m\">%.2f</td>\n"
            "    <td height=\"36\" width=\"25\" port=\"1\">%.2f</td>\n"
            "  </tr>\n"
            "</table>>];\n",
            this, root->red ? "FF000077" : "00000077",
            root->len[0], root->red ? "red" : "black", root->len[1],
            root->tot, root->link_sum[0], root->fit, root->link_sum[1]);

    prev_link = counter;
    for (int i = 0; i < 2; i++) {
      if (link_names[i] == prev_link) {
        continue;
      }
      else {
        fprintf(out, "  s%.3d:%d -> s%.3d;\n", this, i, link_names[i]);
        prev_link = link_names[i];
      }
    }
    fprintf(out, "  s%.3d:m -> s%.3d:0;\n", this, mid);
    return this;
  }
}

void rtree_to_dot(RTree *rt, char *loch) {
  FILE *out = fopen(loch, "w");
  fprintf(out, "digraph g {\n  bgcolor=transparent\n  node [shape=none];\n");
  rtree_to_dot_rec(out, rt->root, 0);
  fprintf(out, "}\n");
  fclose(out);
}

#endif
