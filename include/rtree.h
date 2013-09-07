#ifndef RTREE_H
#define RTREE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct rtree_node_t {
  bool red;
  void *data;
  double fit, tot;
  uint32_t len[2];
  double link_sum[2];
  struct rtree_node_t *link[2];
} rtree_node_t;

typedef struct {
  rtree_node_t *root;
} rtree_t;

rtree_t *rtree_create();
void rtree_destroy(rtree_t *rt);

void rtree_add(rtree_t *rt, void *elt_ptr, double fitness);

void *rtree_rget(rtree_t *rt);
void *rtree_rpop(rtree_t *rt);

uint32_t rtree_length(rtree_t *rt);
double rtree_total_fitness(rtree_t *rt);

#ifdef RTREE_DEBUG

int rb_check(rtree_node_t *root);
double total_fit_check(rtree_t *rt);
double fit_check(rtree_node_t *root);

void rtree_preorder(rtree_t *rt);

void rtree_to_dot(rtree_t *rt, char *loch);

#endif

#endif

