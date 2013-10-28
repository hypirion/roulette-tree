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

#define ITERATIONS 1000

int main() {
  puts("Testing nonuniform frequency.");
  int tests_failed = 0;
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
  if (!(0.1 - 0.01 < val && val < 0.1 + 0.01)) {
    puts("Frequency is too improbable.");
  }
  rtree_destroy(rt);
  return tests_failed != 0;
}
