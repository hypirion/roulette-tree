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
  puts("Testing that the red-black trees keep the red-black invariants.");
  int tests_failed = 0;

  REPEAT (ITERATIONS) {
    RTree *rt = rtree_create();
    uint32_t rounds = (uint32_t) rand() % 50;
    REPEAT (rounds) {
      uint32_t insertions = (uint32_t) rand() % 50;
      REPEAT (insertions) {
        rtree_add(rt, (void *) NULL, ((double) rand()/(double) RAND_MAX) * 15.0);
        if (rb_check(rt->root) <= 0) {
          puts("Red-black violation");
          tests_failed++;
        }
      }
      uint32_t deletions = 0;
      if (rtree_size(rt) > 0) { // Avoid division by zero
        deletions = ((uint32_t) rand()) % rtree_size(rt);
      }
      REPEAT (deletions) {
        rtree_rpop(rt);
        if (rb_check(rt->root) <= 0) {
          puts("Red-black violation");
          tests_failed++;
        }
      }
    }
    rtree_destroy(rt);
  }
  return tests_failed != 0;
}
