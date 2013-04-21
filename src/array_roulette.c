/* Arraylist-implementation of roulette wheel */
#include <string.h>
#include <stdlib.h>
#include "array_roulette.h"

aroulette_t *aroulette_create() {
  aroulette_t *ar = (aroulette_t *) malloc(sizeof(aroulette_t));
  aroulette_elt *alist = (aroulette_elt *)
    malloc(AROULETTE_DEFAULT_CAP * sizeof(aroulette_elt));
  ar->len = 0;
  ar->tot_fitness = 0;
  ar->cap = AROULETTE_DEFAULT_CAP;
  ar->arr = alist;
  return ar;
}

void aroulette_destroy(aroulette_t *ar) {
  free(ar->arr);
  free(ar);
}

void aroulette_add(aroulette_t *ar, void *elt_ptr, double fitness) {
  if (ar->len == ar->cap) { // reallocate the array and double the cap
    ar->cap *= 2;
    aroulette_elt *twice = (aroulette_elt *)
      realloc(ar->arr, ar->cap * sizeof(aroulette_elt));
    ar->arr = twice;
  }
  aroulette_elt *ptr = &(ar->arr[ar->len]);
  ptr->elt_ptr = elt_ptr;
  ptr->fit_from = ar->tot_fitness;
  ar->tot_fitness += fitness;
  ptr->fit_to = ar->tot_fitness;
  ar->len++;
}


int aroulette_bsearch(aroulette_t *a, double pick) {
  int low = 0, high = a->len - 1;
  while (low <= high) {
    int mid = low + (high - low) / 2;
    if (pick < a->arr[mid].fit_from) {
      high = mid - 1;
    }
    else if (pick > a->arr[mid].fit_to) {
      low = mid + 1;
    }
    else {
      return mid;
    }
  }
  return -1;
}

void *aroulette_rget(aroulette_t *ar) {
  double pick = drand48() * ar->tot_fitness;
  int index = aroulette_bsearch(ar, pick);
  return ar->arr[index].elt_ptr;
}

void *aroulette_rpop(aroulette_t *ar) {
  double pick = drand48() * ar->tot_fitness;
  int index = aroulette_bsearch(ar, pick);
  aroulette_elt elt = ar->arr[index];
  double rm_fitness = elt.fit_to - elt.fit_from;
  void *ret = elt.elt_ptr;
  memmove(&(ar->arr[index]), &(ar->arr[index+1]),
          sizeof(aroulette_elt) * (ar->len - index - 1));
  for (int i = index; i < ar->len; i++) {
    ar->arr[i].fit_to -= rm_fitness;
    ar->arr[i].fit_from -= rm_fitness;
  }
  ar->len--;
  ar->tot_fitness -= rm_fitness;
  return ret;
}

uint32_t aroulette_length(aroulette_t *ar) {
  return ar->tot_fitness;
}

double aroulette_total_fitness(aroulette_t *ar) {
  return ar->tot_fitness;
}
