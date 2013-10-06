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

/* Linked-list implementation of fitness-proportionate selection */
#include <stdlib.h>
#include "linked_roulette.h"

Linked *linked_create() {
  Linked *lt = (Linked *) malloc(sizeof(Linked));
  lt->lptr = NULL;
  lt->length = 0;
  lt->tot_fitness = 0;
  return lt;
}

void linked_destroy(Linked *root) {
  linked_ll *ll = root->lptr;
  while (ll != NULL) {
    linked_ll *temp = ll;
    ll = ll->cdr;
    free(temp);
  }
  free(root);
}

void linked_add(Linked *root, void *elt_ptr, double fitness) {
  linked_ll *tail = root->lptr;
  linked_ll *head = (linked_ll *) malloc(sizeof(linked_ll));
  head->car = elt_ptr;
  head->fitness = fitness;
  head->cdr = tail;
  root->lptr = head;
  root->length++;
  root->tot_fitness += fitness;
}

void *linked_rget(Linked *root) {
  double pick = ((double) rand()/(double) RAND_MAX) * root->tot_fitness;
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

void *linked_rpop(Linked *root) {
  double pick = ((double) rand()/(double) RAND_MAX) * root->tot_fitness;
  double accum = 0;
  linked_ll *prev = NULL;
  linked_ll *cur = root->lptr;
  while (1) {
    accum += cur->fitness;
    if (accum < pick) {
      linked_ll *next = cur->cdr;
      prev = cur;
      cur = next;
    }
    else {
      if (prev == NULL) { // Picked first element.
        root->lptr = cur->cdr;
      }
      else { // Kick current away from previous
        prev->cdr = cur->cdr;
      }
      root->length--;
      root->tot_fitness -= cur->fitness;
      void *elt_ptr = cur->car;
      free(cur);
      return elt_ptr;
    }
  }
}

uint32_t linked_length(Linked *root) {
  return root->length;
}

double linked_total_fitness(Linked *root) {
  return root->tot_fitness;
}
