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

#define A 42
#define B 1337
#define SIZE 10000

int main() {
  puts("Testing that duplicates doesn't share probabilities.");
  int tests_failed = 0;

  int count = 0;
  REPEAT(SIZE) {
    RTree *rt = rtree_create();
    REPEAT(3) {rtree_add(rt, (void *) A, 1.0);}
    REPEAT(2) {rtree_add(rt, (void *) B, 1.0);}

    // Picking at least one A out of two means picking no B's.
    // For uniform probability, that is 1 - (2/5 * 1/4) = 0.9
    REPEAT(2) {
      if (rtree_rpop(rt) == (void *) A) {
        count++;
        break;
      }
    }
    rtree_destroy(rt);
  }
  double freq = ((double) count)/SIZE;
  // printf("Frequency: %f\n", freq);
  if (!(0.9 - 0.01 < freq && freq < 0.9 + 0.01)) {
    puts("Frequency too improbable");
    tests_failed++;
  }
  return tests_failed != 0;
}
