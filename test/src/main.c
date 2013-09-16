#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "rtree.h"
#include "minunit.h"
#include "main.h"

int tests_run = 0;

static char *test_all_kept() {
  puts("Testing that inserted elements doesn't disappear or duplicate.");
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

static char *test_consistent_fitness() {
  puts("Testing that the fitness of a randomly mutated rtree is consistent.");
  for (int count = 0; count < 1000; count++) {
    rtree_t *rt = rtree_create();
    uint32_t rounds = (uint32_t) lrand48() % 50;
    while (rounds --> 0) {
      uint32_t insertions = (uint32_t) lrand48() % 50;
      while (insertions --> 0) {
        rtree_add(rt, (void *) NULL, drand48() * 15.0);
        mu_assert("Total fitness doesn't add up", 0.0 <= total_fit_check(rt));
      }
      uint32_t deletions = 0;
      if (rtree_size(rt) > 0) { // Avoid division by zero
        deletions = ((uint32_t) lrand48()) % rtree_size(rt);
      }
      while (deletions --> 0) {
        rtree_rpop(rt);
        mu_assert("Total fitness doesn't add up", 0.0 <= total_fit_check(rt));
      }
    }
    rtree_destroy(rt);
  }
  return 0;
}

static char *test_rb_invariants() {
  puts("Testing that the red-black trees keep the red-black invariants.");
  for (int count = 0; count < 1000; count++) {
    rtree_t *rt = rtree_create();
    uint32_t rounds = (uint32_t) lrand48() % 50;
    while (rounds --> 0) {
      uint32_t insertions = (uint32_t) lrand48() % 50;
      while (insertions --> 0) {
        rtree_add(rt, (void *) NULL, drand48() * 15.0);
        mu_assert("Red-black violation.", rb_check(rt->root) > 0);
      }
      uint32_t deletions = 0;
      if (rtree_size(rt) > 0) { // Avoid division by zero
        deletions = ((uint32_t) lrand48()) % rtree_size(rt);
      }
      while (deletions --> 0) {
        rtree_rpop(rt);
        mu_assert("Red-black violation.", rb_check(rt->root) > 0);
      }
    }
    rtree_destroy(rt);
  }
  return 0;
}

static char *all_tests() {
  mu_run_test(test_all_kept);
  mu_run_test(test_consistent_fitness);
  mu_run_test(test_rb_invariants);
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
