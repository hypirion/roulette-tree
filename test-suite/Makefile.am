AM_CPPFLAGS = -I$(srcdir)/../src
AM_CFLAGS = $(DEBUG_VARS)
LDADD = ../src/librtree.la
check_PROGRAMS =
TESTS =

check_PROGRAMS += test_all_kept
TESTS += test_all_kept
test_all_kept_SOURCES = test_all_kept.c

check_PROGRAMS += test_duplicates
TESTS += test_duplicates
test_duplicates_SOURCES = test_duplicates.c

check_PROGRAMS += test_duplicate_probabilities
TESTS += test_duplicate_probabilities
test_duplicate_probabilities_SOURCES = test_duplicate_probabilities.c

check_PROGRAMS += test_consistent_fitness
TESTS += test_consistent_fitness
test_consistent_fitness_SOURCES = test_consistent_fitness.c

check_PROGRAMS += test_rb_invariants
TESTS += test_rb_invariants
test_rb_invariants_SOURCES = test_rb_invariants.c

check_PROGRAMS += test_nonuniform_frequency
TESTS += test_nonuniform_frequency
test_nonuniform_frequency_SOURCES = test_nonuniform_frequency.c

check_PROGRAMS += test_uniform_frequency
TESTS += test_uniform_frequency
test_uniform_frequency_SOURCES = test_uniform_frequency.c
