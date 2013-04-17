/* Linked-list implementation of fitness-proportionate selection */
#include <stdlib.h>
#include "linked_roulette.h"

linked_t *linked_create() {
  linked_t *lt = (linked_t *) malloc(sizeof(linked_t));
  lt->lptr = NULL;
  lt->length = 0;
  lt->tot_fitness = 0;
  return lt;
}

void linked_destroy(linked_t *root) {
  linked_ll *ll = root->lptr;
  while (ll != NULL) {
    linked_ll *temp = ll;
    ll = ll->cdr;
    free(temp);
  }
  free(root);
}

void linked_add(linked_t *root, void *elt_ptr, double fitness) {
  linked_ll *tail = root->lptr;
  linked_ll *head = (linked_ll *) malloc(sizeof(linked_ll));
  head->car = elt_ptr;
  head->fitness = fitness;
  head->cdr = tail;
  root->lptr = head;
  root->length++;
  root->tot_fitness += fitness;
}

void *linked_rget(linked_t *root) {
  double pick = drand48() * root->tot_fitness;
  double accum = 0;
  linked_ll *cur = root->lptr;
  while (1) {
    accum += cur->fitness;
    if (accum < pick) {
      cur = cur->cdr;
    }
    else {
      return cur->car;
    }
  }
}