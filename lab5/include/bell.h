/*  bell.h
    ------
    Header for the library for generating Bell numbers
*/

#ifndef _BELL_H_
#define _BELL_H_

#include <combinatorics.h>



// DESCRIPTION
//   bell() computes Bell numbers (numbers in OEIS sequence A000110).
//   Bell number B_n describes the number of ways to partitions
//   a set of n objects.
// INPUT
//    n - the sequence number of the Bell number B_n to compute
// OUTPUT
//    ans - a pointer to a variable of type comb_t, which
//         will be populated with B_n
// RETURN VALUE
//    The function returns 0 on success, or -1 on error.
//
int bell(int n, comb_t *ans);


#endif
