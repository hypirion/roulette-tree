#ifndef LINKED_ROULETTE_H
#define LINKED_ROULETTE_H

typedef struct ll {
  void *car;
  struct ll *cdr;
} linked_ll;

typedef struct {
  linked_ll *lptr;
} linked_t;

void linked_add(linked_t *root, void *elt_ptr);

void *linked_rget(linked_t *root);
void *linked_rpop(linked_t *root);

void *linked_size(linked_t *root);
#endif
