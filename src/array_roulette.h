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

#ifndef ARRAY_ROULETTE_H
#define ARRAY_ROULETTE_H
#include <stdint.h>

#define AROULETTE_DEFAULT_CAP 32

typedef struct {
  void *elt_ptr;
  double fit_from, fit_to;
} ARouletteElt;

typedef struct {
  ARouletteElt *arr;
  uint32_t len, cap;
  double tot_fitness;
} ARoulette;

ARoulette *aroulette_create();
void aroulette_destroy(ARoulette *ar);

void aroulette_add(ARoulette *ar, void *elt_ptr, double fitness);

void *aroulette_rget(ARoulette *ar);
void *aroulette_rpop(ARoulette *ar);

uint32_t aroulette_length(ARoulette *ar);
double aroulette_total_fitness(ARoulette *ar);
#endif
