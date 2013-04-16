# Roulette tree

A roulette tree (from now on abbreviated *rtree*) is a data structure which
allows one to pick a random item based on relative frequency in logarithmic
time. If you for instance have three items: A, B and C, and A has a frequency of
1, B has a frequency of 2 and C has a frequency of 3, then a random pick would
pick A 1/6 of the time, B 1/3 of the time, and C 1/2 of the time.

The rtree is designed to do insertions, random picks and random pops in
O(log n) time, and count and total frequency in O(1) time.
