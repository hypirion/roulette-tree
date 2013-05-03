#include <stdio.h>
#include "rtree.h"

int main(int argc, char **argv) {
  rtree_t *rt = rtree_create();
  for (int i = 0; i < 8; i++) {
    rtree_add(rt, (void *) ((long) (i + 1)), (double) (i+1));
    rb_check(rt->root);
    total_fit_check(rt);
    rtree_preorder(rt);
    printf("\n");
  }
  printf("Now removing items...\n\n");
  for (int i = 0; i < 8; i++) {
    char outfile[80];
    sprintf(outfile, "imgs/test-%.2d.dot", i);
    rtree_to_dot(rt, outfile);
    long l = (long) rtree_rpop(rt);
    printf("Got back %ld\n", l);
    rb_check(rt->root);
    total_fit_check(rt);
    //    rtree_preorder(rt);
    printf("\n");
  }
  rtree_destroy(rt);
  return 0;
}
