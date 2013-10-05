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

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "rtree.h"

int main(int argc, char **argv) {
  long seed = ((long) time(0)) << 16;
  seed48((short unsigned int *)&seed);
  rtree_t *rt = rtree_create();
  for (int i = 0; i < 12; i++) {
    rtree_add(rt, (void *) ((long) (i + 1)), ((double)rand()/(double)RAND_MAX));
  }
  printf("Now removing items...\n\n");
  for (int i = 0; i < 12; i++) {
    char outfile[80];
    sprintf(outfile, "imgs/test-%.2d.dot", i);
    rtree_to_dot(rt, outfile);
    long l = (long) rtree_rpop(rt);
    printf("Got back %ld\n", l);
  }
  rtree_destroy(rt);
  return 0;
}
