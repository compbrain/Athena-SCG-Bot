#include <stdio.h>
#include <stdlib.h>
#include "minunit.h"
#include "poly.h"

int tests_run = 0;
int asserts = 0;

#define DELTA 0.001
#define IN_RANGE(actual, expected) \
    (((expected - DELTA) <= actual) && (actual <= (expected + DELTA)))

static char *test_find_break_even() {
    uint32_t rn;

    /* Odd relationship numbers all have the same deal.
     * Set everything to false. */
    for (rn = 1; rn < 256; rn += 2) {
        mu_assert("broke, damn it",
                  IN_RANGE(find_break_even(rn, 3), 1.0));
    }
    /* Relationship numbers above 128 all have the same deal.
     * Set everything to true. */
    for (rn = 128; rn < 256; rn++) {
        mu_assert("broke, damn it",
                  IN_RANGE(find_break_even(rn, 3), 1.0));
    }
    /* http://www.ccs.neu.edu/home/lieber/courses/cs4500/f09/preparation/break-even-table
    0 : 0.0 : 0.0
    1 : 0.0 : 1.0
    2 : 0.3333333333333333 : 0.14814814814814814 = 4/27
    3 : 0.0 : 1.0
    4 : 0.3333333333333333 : 0.14814814814814814 = 4/27
    5 : 0.0 : 1.0
    6 : 0.3333333333333333 : 0.2962962962962963 = 8/27
    7 : 0.0 : 1.0
    8 : 0.6666666666666666 : 0.14814814814814814
    9 : 0.0 : 1.0
    10 : 0.5 : 0.25
    11 : 0.0 : 1.0
    12 : 0.5 : 0.25
    13 : 0.0 : 1.0
    14 : 0.42264973081037427 : 0.38490017945975047
    15 : 0.0 : 1.0
    16 : 0.3333333333333333 : 0.14814814814814814
    17 : 0.0 : 1.0
    18 : 0.3333333333333333 : 0.2962962962962963
    19 : 0.0 : 1.0
    20 : 0.3333333333333333 : 0.2962962962962963
    21 : 0.0 : 1.0
    22 : 0.3333333333333333 : 0.4444444444444445
    */

    mu_assert("RN 2", IN_RANGE(find_break_even(2, 3), 0.148148148148));
    mu_assert("RN 4", IN_RANGE(find_break_even(4, 3), 0.148148148148));
    mu_assert("RN 6", IN_RANGE(find_break_even(6, 3), 0.296296296296));
    mu_assert("RN 8", IN_RANGE(find_break_even(8, 3), 0.148148148148));
    mu_assert("RN 10", IN_RANGE(find_break_even(10, 3), 0.25));
    mu_assert("RN 12", IN_RANGE(find_break_even(12, 3), 0.25));
    mu_assert("RN 14", IN_RANGE(find_break_even(14, 3), 0.384900179));
    mu_assert("RN 16", IN_RANGE(find_break_even(16, 3), 0.148148148148));
    mu_assert("RN 18", IN_RANGE(find_break_even(18, 3), 0.296296296296));
    mu_assert("RN 20", IN_RANGE(find_break_even(20, 3), 0.296296296296));
    mu_assert("RN 22", IN_RANGE(find_break_even(22, 3), 0.444444444444));

    return NULL;
}

/* poly tests */

static char *test_poly_new() {
    poly *p;

    p = poly_new(3, 17, 3, 2, 1);
    mu_assert("p == NULL", p != NULL);

    mu_assert("p(0.0) != 17.0", IN_RANGE(poly_eval(p, 0), 17));
    mu_assert("p(1.0) != 23.0", IN_RANGE(poly_eval(p, 1), 23));
    mu_assert("p(2.0) != 39.0", IN_RANGE(poly_eval(p, 2), 39)); 

    poly_delete(p);
    return NULL;
}

static char *test_poly_synth_div() {
    poly *p;

    p = poly_new(5, -1, 0, 25, 0, 3, 2);
    mu_assert("2x^5 + 3x^4 + 25x^2 - 1", poly_synth_div(p, -3) == -19);
    poly_delete(p);

    p = poly_new(3, 2, 1, -8, 1);
    mu_assert("x^3 - 8x^2 + x + 2", poly_synth_div(p, 7) == -40);
    poly_delete(p);

    p = poly_new(2, 1, 2, 1);
    mu_assert("(x+1)(x+1)", poly_synth_div(p, -1) == 0);
    poly_delete(p);

    return NULL;
}

static char *test_poly_maxima() {
    poly *p;

    p = poly_new(4, 0, 0, 0, 0, 1);
    mu_assert("x^4 from 0.0 to 1.0", IN_RANGE(poly_maxima(p, 0, 1), 1.0));
    mu_assert("x^4 from 1.0 to 2.0", IN_RANGE(poly_maxima(p, 1, 2), 16.0));
    poly_delete(p);

    p = poly_new(4, 1, 1, -3, 4, 3);
    mu_assert("3x^4 + 4x^3 - 3x^2 + x + 1 from 0.0 to 1.0",
              IN_RANGE(poly_maxima(p, 0, 1), 6.0));
    mu_assert("3x^4 + 4x^3 - 3x^2 + x + 1 from 1.0 to 2.0",
              IN_RANGE(poly_maxima(p, 1, 2), 71.0));
    poly_delete(p);

    p = poly_new(3, 1, -3, 3, -1);
    mu_assert("-x^3 + 3x^2 - 3x + 1", IN_RANGE(poly_maxima(p, 0, 1), 1.0));
    poly_delete(p);

    p = poly_new(3, 0, 1, -2, 1);
    mu_assert("x^3 - 2x^2 + x", IN_RANGE(poly_maxima(p, 0, 1), 0.148148));
    poly_delete(p);

    p = poly_new(3, 0, 0, 1, -1);
    mu_assert("-x^3 + x^2", IN_RANGE(poly_maxima(p, 0, 1), 0.148148));
    poly_delete(p);

    p = poly_new(3, 0, 0, 0, 1);
    mu_assert("x^3", IN_RANGE(poly_maxima(p, 0, 1), 1.0));
    poly_delete(p);
    return NULL;
}

static char *test_pascal() {
    mu_assert("0,0", pascal(0, 0) == 1);
    mu_assert("1,0", pascal(1, 0) == 1);
    mu_assert("2,0", pascal(2, 0) == 1);
    mu_assert("3,3", pascal(3, 3) == 1);
    mu_assert("4,2", pascal(4, 2) == 6);
    mu_assert("5,3", pascal(5, 3) == 10);
    mu_assert("7,1", pascal(7, 1) == 7);
    return NULL;
}

static char *test_poly_from_relation_number() {
    poly *p;

#define POLY_FROM_MACRO(rn, rank, a0, a1, a2, a3, a4, a5) do { \
    p = poly_from_relation_number(rn, rank); \
    mu_assert(#rn " 0 " #a0, p->coeffs[0] == a0); \
    if (1 <= rank) mu_assert(#rn " 1 " #a1, p->coeffs[1] == a1); \
    if (2 <= rank) mu_assert(#rn " 2 " #a2, p->coeffs[2] == a2); \
    if (3 <= rank) mu_assert(#rn " 3 " #a3, p->coeffs[3] == a3); \
    if (4 <= rank) mu_assert(#rn " 4 " #a4, p->coeffs[4] == a4); \
    if (5 <= rank) mu_assert(#rn " 5 " #a5, p->coeffs[5] == a5); \
    poly_delete(p); } while(0)
#define POLY_FROM_MACRO3(rn, a0, a1, a2, a3) \
    POLY_FROM_MACRO(rn, 3, a0, a1, a2, a3, 0, 0);
#define POLY_FROM_MACRO4(rn, a0, a1, a2, a3, a4) \
    POLY_FROM_MACRO(rn, 4, a0, a1, a2, a3, a4, 0);
#define POLY_FROM_MACRO5(rn, a0, a1, a2, a3, a4, a5) \
    POLY_FROM_MACRO(rn, 5, a0, a1, a2, a3, a4, a5);

    POLY_FROM_MACRO3(1,   1, -3,  3, -1);
    POLY_FROM_MACRO3(2,   0,  1, -2,  1);
    POLY_FROM_MACRO3(22,  0,  3, -6,  3);
    POLY_FROM_MACRO3(60,  0,  2, -2,  0);
    POLY_FROM_MACRO3(16,  0,  1, -2,  1);
    POLY_FROM_MACRO3(128, 0,  0,  0,  1);

    POLY_FROM_MACRO4(514, 0, 1, -2, 1, 0);
    POLY_FROM_MACRO4(2222, 0, 2, -4, 4, -2);

    return NULL;
}

static char *test_poly_eval() {
    poly *p;
    
    p = poly_new(7, 1, 0, 0, 1, 0, 0, 0, 1);
    mu_assert("x^7 + x^3 + 1, x = 0", IN_RANGE(poly_eval(p, 0), 1));
    mu_assert("x^7 + x^3 + 1, x = 1", IN_RANGE(poly_eval(p, 1), 3));
    mu_assert("x^7 + x^3 + 1, x = 2", IN_RANGE(poly_eval(p, 2), 128 + 8 + 1));
    poly_delete(p);
    return NULL;
}

char *all_tests() {
    mu_run_test(test_find_break_even);
    mu_run_test(test_poly_new);
    mu_run_test(test_poly_synth_div);
    mu_run_test(test_poly_maxima);
    mu_run_test(test_pascal);
    mu_run_test(test_poly_from_relation_number);
    mu_run_test(test_poly_eval);
    return NULL;
}
