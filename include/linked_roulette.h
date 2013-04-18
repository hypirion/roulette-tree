#ifndef LINKED_ROULETTE_H
#define LINKED_ROULETTE_H
#include <stdint.h>

typedef struct ll {
  void *car;
  double fitness;
  struct ll *cdr;
} linked_ll;

typedef struct {
  linked_ll *lptr;
  uint32_t length;
  double tot_fitness;
} linked_t;

linked_t *linked_create();
void linked_destroy(linked_t *root);

void linked_add(linked_t *root, void *elt_ptr, double fitness);

void *linked_rget(linked_t *root);
void *linked_rpop(linked_t *root);

uint32_t linked_length(linked_t *root);
#endif
