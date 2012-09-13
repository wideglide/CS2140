/*  lucas.h
    -------
    Header for the library for generating Lucas numbers
*/

#ifndef _LUCAS_H_
#define _LUCAS_H_

#include <combinatorics.h>

//
// struct for storing parameters for generalized Lucas sequences
//   Given L(n) = p*L(n-1) - q*L(n-2)
// members are:
//    l0 - the value of L(0)
//    l1 - the value of L(1)
//    p  - the value of p
//    q  - the value of q
//
// For example {2 1 1 -1} gives the Lucas numbers,
// and {1 1 1 -1} gives the Fibonacci numbers
//
typedef struct {
    comb_t l0;
    comb_t l1;
    comb_t p;
    comb_t q;
} linfo_t;


// DESCRIPTION
//   lucas() computes generalized Lucas sequence numbers.
// INPUT
//    gen - a pointer to generalized Lucas sequecne parameters
//    n - the sequence number of the Lucas number to compute
// OUTPUT
//    ans - a pointer to a variable of type comb_t, which
//         will be populated with the requested sequence number
// RETURN VALUE
//    The function returns 0 on success, or -1 on error.
//
int lucas(linfo_t *gen, int n, comb_t *ans);

#endif

