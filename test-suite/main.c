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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "rtree.h"
#include "minunit.h"
#include "main.h"

#define __GENSYM2(x,y) x##y
#define __GENSYM1(x,y) __GENSYM2(x,y)
#define GENSYM(x) __GENSYM1(x,__COUNTER__)

#define FOR(name, n) for (int name = 0; name < n; name++)
#define REPEAT(n) FOR(GENSYM(iter), n)

int tests_run = 0;
int tests_failed = 0;

static int test_duplicates() {
  puts("Testing that duplicates are allowed.");
  RTree *rt = rtree_create();
  const int elts = 10;
  const int nof_insertions = 10;
  for (uintptr_t elt = 0; elt < elts; elt++) {
    REPEAT (nof_insertions) {
      rtree_add(rt, (void *) elt, 1.0);
    }
  }
  int *count = calloc(10, sizeof(int));
  while (rtree_size(rt) > 0) {
    count[(uintptr_t) rtree_rpop(rt)]++;
  }
  for (uintptr_t elt = 0; elt < elts; elt++) {
    char *msg = calloc(80, sizeof(char));
    sprintf(msg, "Expected %d elements for %ld, not %d.", nof_insertions, elt,
            count[elt]);
    mu_assert(msg, count[elt] == nof_insertions);
    free(msg);
  }
  free(count);
  return 0;
}

static int test_duplicate_probabilities() {
  puts("Testing that duplicates doesn't share probabilities.");
  const uintptr_t a = 42;
  const uintptr_t b = 1337;
  const int size = 10000;
  int count = 0;
  REPEAT(size) {
    RTree *rt = rtree_create();
    REPEAT(3) {rtree_add(rt, (void *) a, 1.0);}
    REPEAT(2) {rtree_add(rt, (void *) b, 1.0);}

    // Picking at least one a out of two means picking no b's.
    // For uniform probability, that is 1 - (2/5 * 1/4) = 0.9
    REPEAT(2) {
      if (rtree_rpop(rt) == (void *) a) {
        count++;
        break;
      }
    }
    rtree_destroy(rt);
  }
  double freq = ((double) count)/size;
  // printf("Frequency: %f\n", freq);
  mu_assert("Frequency too improbable", 0.9 - 0.01 < freq && freq < 0.9 + 0.01);
  return 0;
}

static int test_consistent_fitness() {
  puts("Testing that the fitness of a randomly mutated rtree is consistent.");
  for (int count = 0; count < 1000; count++) {
    RTree *rt = rtree_create();
    uint32_t rounds = (uint32_t) rand() % 50;
    REPEAT (rounds) {
      uint32_t insertions = (uint32_t) rand() % 50;
      REPEAT (insertions) {
        rtree_add(rt, (void *) NULL, ((double) rand()/(double) RAND_MAX) * 15.0);
        mu_assert("Total fitness doesn't add up", 0.0 <= total_fit_check(rt));
      }
      uint32_t deletions = 0;
      if (rtree_size(rt) > 0) { // Avoid division by zero
        deletions = ((uint32_t) rand()) % rtree_size(rt);
      }
      REPEAT (deletions) {
        rtree_rpop(rt);
        mu_assert("Total fitness doesn't add up", 0.0 <= total_fit_check(rt));
      }
    }
    rtree_destroy(rt);
  }
  return 0;
}

static int test_rb_invariants() {
  puts("Testing that the red-black trees keep the red-black invariants.");
  for (int count = 0; count < 1000; count++) {
    RTree *rt = rtree_create();
    uint32_t rounds = (uint32_t) rand() % 50;
    REPEAT (rounds) {
      uint32_t insertions = (uint32_t) rand() % 50;
      REPEAT (insertions) {
        rtree_add(rt, (void *) NULL, ((double) rand()/(double) RAND_MAX) * 15.0);
        mu_assert("Red-black violation.", rb_check(rt->root) > 0);
      }
      uint32_t deletions = 0;
      if (rtree_size(rt) > 0) { // Avoid division by zero
        deletions = ((uint32_t) rand()) % rtree_size(rt);
      }
      REPEAT (deletions) {
        rtree_rpop(rt);
        mu_assert("Red-black violation.", rb_check(rt->root) > 0);
      }
    }
    rtree_destroy(rt);
  }
  return 0;
}

static int test_nonuniform_frequency() {
  puts("Testing nonuniform frequency.");
  RTree *rt = rtree_create();
  rtree_add(rt, (void *) ((uintptr_t) 0), 1.0);
  rtree_add(rt, (void *) ((uintptr_t) 1), 9.0);
  const int picks = 10000;
  int count[2] = {0, 0};
  for (int i = 0; i < picks; i++) {
    count[(uintptr_t) rtree_rget(rt)]++;
  }
  // printf("Frequency for 0.1 value: %.4f\n", ((double) count[0])/picks);
  // printf("Frequency for 0.9 value: %.4f\n", ((double) count[1])/picks);
  double val = ((double) count[0])/picks;
  // TODO: 99% confidence intervals? need something more sane here.
  mu_assert("Frequency is too improbable.",
            0.1 - 0.01 < val && val < 0.1 + 0.01);
  rtree_destroy(rt);
  return 0;
}

static int test_uniform_frequency() {
  puts("Testing uniform frequency.");
  RTree *rt = rtree_create();
  const int elems = 100;
  for (uintptr_t i = 0; i < elems; i++) {
    rtree_add(rt, (void *) i, 1.0);
  }
  const int size = 100000;
  const double prob = 1.0/((double) elems);
  int *count = calloc(elems, sizeof(int));
  for (int i = 0; i < size; i++) {
    count[(uintptr_t) rtree_rget(rt)]++;
  }
  for (int i = 0; i < elems; i++) {
    double freq = ((double) count[i])/size;
    if (!(prob - 0.005 < freq && freq < prob + 0.005)) {
      printf("[FAIL] Expected frequency for %d to be within %.4f and %.4f, was "
             "%.4f.\n", i, prob - 0.005, prob + 0.005, freq);
      return 1;
    }
  }
  free(count);
  rtree_destroy(rt);
  return 0;
}

static void all_tests() {
  mu_run_test(test_duplicates);
  mu_run_test(test_duplicate_probabilities);
  mu_run_test(test_consistent_fitness);
  mu_run_test(test_rb_invariants);
  mu_run_test(test_nonuniform_frequency);
  mu_run_test(test_uniform_frequency);
}

int main() {
  all_tests();
  if (tests_failed) {
    printf("%d test%s failed.\n", tests_failed, tests_failed == 1 ? "" : "s");
  }
  else {
    printf("All tests passed.\n");
  }
  printf("Tests run: %d\n", tests_run);

  return tests_failed != 0;
}
