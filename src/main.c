#include <stdio.h>
#include "rtree.h"

int main(int argc, char **argv) {
  rtree_t *rt = rtree_create();
  for (int i = 0; i < 12; i++) {
    rtree_add(rt, (void *) ((long) (i + 1)), (double) drand48());
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
