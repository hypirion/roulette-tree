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
#include "test.h"

#define ELEMS 100
#define SIZE 100000

int main() {
  puts("Testing uniform frequency.");
  int tests_failed = 0;

  RTree *rt = rtree_create();
  for (uintptr_t i = 0; i < ELEMS; i++) {
    rtree_add(rt, (void *) i, 1.0);
  }

  const double prob = 1.0/((double) ELEMS);
  int count[ELEMS] = {0};
  for (int i = 0; i < SIZE; i++) {
    count[(uintptr_t) rtree_rget(rt)]++;
  }

  for (int i = 0; i < ELEMS; i++) {
    double freq = ((double) count[i])/SIZE;
    if (!(prob - 0.005 < freq && freq < prob + 0.005)) {
      printf("[FAIL] Expected frequency for %d to be within %.4f and %.4f, was "
             "%.4f.\n", i, prob - 0.005, prob + 0.005, freq);
      tests_failed++;
    }
  }

  rtree_destroy(rt);
  return tests_failed != 0;
}
