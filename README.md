# Roulette tree

A roulette tree (abbreviated *rtree*) is a data structure which allows one to
pick a random item based on relative frequency in logarithmic time. If you for
instance have three items: A, B and C, and A has a frequency of 1, B has a
frequency of 2 and C has a frequency of 3, then a random pick would pick A 1/6
of the time, B 1/3 of the time, and C 1/2 of the time. Note that an rtree is not
a set, it is an unordered collection of elements, where each element has a
fitness value.

The rtree is designed to do insertions, random picks and random pops in
O(log n) time, and count and total frequency in O(1) time.

## Rationale

A roulette tree is an excellent data structure for picking random elements based
on fitness in evolutionary algorithms, especially when the set of individuals is
huge. Using an array-based roulette tree makes insertion O(1) and random lookup
O(log n), but random popping will still be O(n). Using linked lists will give
insertion in O(1), lookup is O(n) and random popping in O(n). The roulette tree
has a O(log n) insertion, O(log n) lookup and O(log n) random pop, considerably
better if random popping is frequently done.

If random popping is done to select individuals for the next generation, the
roulette tree will give a O(n log n) update speed, compared to O(n^2) for the
other structures.

## Usage

A roulette tree is rather easy to use. As an example, consider an algorithm
where you have an *n+m*-sized population, and you pick *n* individuals from the
old generation and *m* individuals from the new one. Assuming that the old
population is contained within a rtree and the new individuals in an array of
size *n+m*, a function to create a new population may look like this:

```c
void destroy_genotype_rtree(rtree_t *rt) {
    // If you don't need to free memory, you can skip this loop and just do
    // rtree_destroy(rt)
    while (rtree_size(rt)) {
      genotype *g = (genotype *) rtree_rpop(rt);
      genotype_destroy(g);
    }
    rtree_destroy(rt);
}

rtree_t *make_gen(rtree_t *old_gen, genotype *new_gen_arr, int n, int m) {
    rtree_t *combined = rtree_create();

    // Add n individials from old gen
    for (int i = 0; i < n; i++) {
        genotype *chosen = (genotype *) rtree_rpop(old_gen);
        rtree_add(combined, (void *) chosen, calc_fitness(chosen));
    }
    destroy_genotype_rtree(old_gen);

    // insert new gen into its own rtree
    rtree_t *new_gen = rtree_create();
    for (int i = 0; i < m+n; i++) {
        double fitness = calc_fitness(new_gen_arr[i]);
        rtree_add(new_gen, (void *) new_gen_arr[i], fitness);
    }
    // pick m elements from new gen
    for (int i = 0; i < m; i++) {
        genotype *chosen = (genotype *) rtree_rpop(new_gen);
        rtree_add(combined, (void *) chosen, calc_fitness(chosen));
    }
    destroy_genotype_rtree(new_gen);
    return combined;
}
```

# License

Copyright © 2013 Jean Niklas L'orange

Distributed under the MIT License (MIT).
