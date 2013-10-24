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

#define ELTS 10
#define NOF_INSERTIONS 10

int main() {
  puts("Testing that duplicates are allowed.");
  int tests_failed = 0;

  RTree *rt = rtree_create();
  for (uintptr_t elt = 0; elt < ELTS; elt++) {
    REPEAT (NOF_INSERTIONS) {
      rtree_add(rt, (void *) elt, 1.0);
    }
  }
  int count[ELTS] = {0};
  while (rtree_size(rt) > 0) {
    count[(uintptr_t) rtree_rpop(rt)]++;
  }
  rtree_destroy(rt);
  for (uintptr_t elt = 0; elt < ELTS; elt++) {
    if (count[elt] != NOF_INSERTIONS) {
      printf("Expected %d elements for %ld, not %d.", NOF_INSERTIONS, elt,
              count[elt]);
      tests_failed++;
    }
  }
  return tests_failed != 0;
}
