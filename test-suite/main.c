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
