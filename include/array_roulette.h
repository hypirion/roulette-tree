#ifndef ARRAY_ROULETTE_H
#define ARRAY_ROULETTE_H
#include <stdint.h>

#define AROULETTE_DEFAULT_CAP 32

typedef struct {
  void *elt_ptr;
  double fit_from, fit_to;
} aroulette_elt;

typedef struct {
  aroulette_elt *arr;
  uint32_t len, cap;
  double tot_fitness;
} aroulette_t;

aroulette_t *aroulette_create();
void aroulette_destroy(aroulette_t *ar);

void aroulette_add(aroulette_t *ar, void *elt_ptr, double fitness);

void *aroulette_rget(aroulette_t *ar);
void *aroulette_rpop(aroulette_t *ar);

uint32_t aroulette_length(aroulette_t *ar);
#endif
