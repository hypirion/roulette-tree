#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "rtree.h"
#include "minunit.h"
#include "main.h"

int tests_run = 0;

static char *test_all_kept() {
  for (int count = 0; count < 1000; count++) {
    rtree_t *rt = rtree_create();
    for (uintptr_t i = 0; i < count; i++) {
      rtree_add(rt, (void *) i, (double) drand48());
    }
    char *recvd = calloc(count, sizeof(char));
    for (int i = 0; i < count; i++) {
      int pos = (int) ((uintptr_t) rtree_rpop(rt));
      recvd[pos]++;
    }
    for (int i = 0; i < count; i++) {
      char *err_msg = calloc(80, sizeof(char));
      sprintf(err_msg, "Expected exactly 1 elt with value %d, but was %d\n", i,
              recvd[i]);
      mu_assert(err_msg, recvd[i] == 1);
      free(err_msg);
    }
    free(recvd);
    rtree_destroy(rt);
  }
  return 0;
}

static char *all_tests() {
  mu_run_test(test_all_kept);
  return 0;
}

int main() {
  char *result = all_tests();
  if (result != 0) {
    printf("%s\n", result);
  }
  else {
    printf("All tests passed.\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
