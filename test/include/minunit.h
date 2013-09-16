#define mu_assert(message, test) do { if (!(test)) \
      { printf("[FAIL] %s\n", message); tests_failed++; return 1;}} while (0)
#define mu_run_test(test) do { test(); tests_run++; } while (0)

extern int tests_run;
extern int tests_failed;
