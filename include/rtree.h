#ifndef RTREE_H
#define RTREE_H

#include <stdint.h>

typedef struct {
  uint32_t len;
  double tot_fitness; // (?)
} rtree_t;

rtree_t *rtree_create();
void rtree_destroy(rtree_t *rt);

void rtree_add(rtree_t *ar, void *elt_ptr, double fitness);

void *rtree_rget(rtree_t *ar);
void *rtree_rpop(rtree_t *ar);

uint32_t rtree_length(rtree_t *ar);
double rtree_total_fitness(rtree_t *ar);
#endif

