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

static rtree_node_t *rot_twice(rtree_node_t *root, int dir) {
  root->link[!dir] = rot_once(root->link[!dir], !dir);
  return rot_once(root, dir);
}

static rtree_node_t *rtree_node_create(void *data, double fitness) {
  rtree_node_t *node = (rtree_node_t *) malloc(sizeof(rtree_node_t));
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
        q->len[dir]++;
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

static double rtree_find_fit(rtree_t *rt, double pick) {
  rtree_node_t *cur = rt->root;
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

void *rtree_rpop(rtree_t *rt) { // Can assume that there is at least one elem
  rtree_node_t *cur = rt->root;
  double pick = drand48() * cur->tot;
  const double fit_to_remove = rtree_find_fit(rt, pick);
  void *data_ptr = NULL;
  if (rt->root != NULL) {
    rtree_node_t head = {0};
    double fit_left = pick;

    rtree_node_t *q, *p, *g;
    rtree_node_t *f = NULL; /* the found item */
    bool found = false;
    int dir = 1;

    q = &head;
    g = p = NULL;
    q->link[1] = rt->root;

    while (q->link[dir] != NULL) {
      int last = dir;

      g = p, p = q;
      if (!found) {
        q->link_sum[dir] -= fit_to_remove;
        q->tot -= fit_to_remove;
      }

      q = q->link[dir];

      /* which direction to go */
      if (q->link_sum[0] < fit_left) { // right or this one
        fit_left -= q->link_sum[0];
        dir = 1;
        if (pick < cur->fit) { // save found node
          f = q;
          found = true;
        }
        else { // right
          fit_left -= q->fit;
        }
      }
      else { // go left
        dir = 0;
      }

      /* Push red node down */
      if (is_red(q) && !is_red(q->link[dir])) {
        if (is_red(q->link[!dir])) {
          p = p->link[last] = rot_once(q, dir);
        }
        else if (!is_red(q->link[!dir])) {
          rtree_node_t *s = p->link[!last];

          if (s != NULL) {
            if (!is_red(s->link[!last]) && !is_red(s->link[last])) {
              /* basic colour flip here */
              p->red = false;
              s->red = true;
              q->red = true;
            }
            else {
              int d2 = (g->link[1] == p);
              if (is_red(s->link[last])) {
                g->link[d2] = rot_twice(p, last);
              }
              else if (is_red(s->link[!last])) {
                g->link[d2] = rot_once(p, last);
              }

              /* Set correct colours */
              q->red = g->link[d2]->red = true;
              g->link[d2]->link[0]->red = false;
              g->link[d2]->link[1]->red = false;
            }
          }
        }
      }
    }

    if (f != NULL) {
      data_ptr = f->data;

      // If we haven't been able to bubble down a red node, colour flip a bit
      // more
      if (!is_red(q)) {
        rtree_node_t *s = p->link[p->link[1] != q];
        p->red = false;
        s->red = true;
        // TODO: This is probably not sufficient.
      }

      // Bubbling this value upwards.. May not be as pretty as we'd like it to.
      f->data = q->data;
      /* Set p's link to q to q's child, if any exist. */
      p->link[p->link[1] == q] = q->link[q->link[0] == NULL];


      free(q);
    }
    else {
      // If this happens, then I've done something horribly wrong.
    }

    rt->root = head.link[1];
    if (rt->root != NULL) {
      rt->root->red = false;
    }
  }
  return data_ptr;
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

static int rtree_to_dot_rec(FILE *const out, rtree_node_t *const root,
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
    fprintf(out, "  s%.3d [label=\"%ld\"];\n", mid, (long) root->data);
    const int this = mid + 1;
    fprintf(out, "  s%.3d [label=\"{{%s | %d | %d} | %.2f |"
                 "{<0>%.2f |<m>%.2f | <1>%.2f}}\"];\n",
            this,  root->red ? "red" : "black", root->len[0], root->len[1],
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
    fprintf(out, "  s%.3d:m -> s%.3d;\n", this, mid);
    return this;
  }
}

void rtree_to_dot(rtree_t *rt, char *loch) {
  FILE *out = fopen(loch, "w");
  fprintf(out, "digraph g {\n  node [shape=record];\n");
  rtree_to_dot_rec(out, rt->root, 0);
  fprintf(out, "}\n");
  fclose(out);
}

#endif
