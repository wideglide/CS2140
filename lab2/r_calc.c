//============================================================================
// Name        : r_calc.c
// Author      : Bundt, Joshua
// Version     : 1.0 - Lab 2 - CS 2140
// Copyright   : Naval Postgraduate School
// Description : Return the Lucas and Bell numbers given integer input
//============================================================================

#include <stdio.h>
#include <stdlib.h>

int lucas (int n);
int bell (int i, int j);

int main ( int argc, const char* argv[] ) {
	int i, inputN;
	
	/* Check for the existence of at least one argument*/
	if (argc < 2) {
		printf("--- error - at least one integer command argument is required.\n");
		return -1;
	}
	
	printf("\n");
	for (i = 1; i < argc; ++i)  {
	
		inputN = atoi(argv[i]);
		/* Check result of integer conversion for sensible values
		 *  anything above 15 is unreasonable for Bell numbers.*/
		if (inputN < 0  || inputN > 15) {
			printf("--- error - input value out of range ( 0 < i > 15)\n\n");
			continue;
		}
		printf("L_%d = %d \n\n", inputN, lucas(inputN));
		printf("B_%d = %d \n\n", inputN, bell(inputN, inputN));
		
	}
	return 0;
}

/* The Lucas numbers are closely related to the Fibonacci numbers. 
 * The n-th Lucas number Ln is calculated as L(n) using the 
 * following recurrence. 
 *   L(0) = 2 
 *   L(1) = 1 
 *   L(n) = L(n − 1) + L(n − 2) for all n > 1  */
 
int lucas (int n) {
	
	if (n == 0) {
		return 2;
	}
	if (n == 1) return 1;
	return (lucas (n-1) + lucas (n-2));
}

/* The n-th Bell number Bn is the number of ways of partitioning
 * n different objects. Bn can be calculated as B(n, n) 
 * using the following recurrence. 
 * 	 B(0, 0) = B(1, 1) = 1 
 * 	 B(n, 1) = B(n − 1, n − 1) for all n > 1 
 * 	 B(i, j) = B(i − 1, j − 1) + B(i, j − 1) for all n > 1, i >= j > 1  */

int bell (int i, int j) {
	
	if ((i == 0 && j == 0) || (i == 1 && j == 1)) 
		return 1;

	if (j == 1) return bell(i-1, i-1);
	return (bell (i-1, j-1) + bell (i, j-1));
}
