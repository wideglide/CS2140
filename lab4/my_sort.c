//============================================================================
// Name        : my_sort.c
// Author      : Bundt, Joshua
// Version     : 1.0 - Lab 4 - CS 2140
// Copyright   : Naval Postgraduate School
// Description : Compare sorting alorthims
//============================================================================


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

// swap function used in merge sort to swap two elements in array
void swap(int *a,int *b) {
	int temp;
	temp = *a;
	*a = *b;
	*b = temp;
}


// 
// name: Insertion Sort - performs the Insertion sort algorithm on an array
//          of intergers
// @param
//   a - a pointer to an array to be sorted
//   len - length of array
//

void insertion_sort(int *a, int len) {
	int i, j, key;
	
	for (j = 1; j < len; j++) {
		key = a[j];
		i = j - 1;
		while (i >=0 && a[i] > key) {
			a[i+1] = a[i];
			i--;
		}
		a[i+1] = key;
	}
}


// 
// name: Bubble Sort - performs the bubble sort algorithm on an array
//          of intergers
// @param
//   a - a pointer to an array to be sorted
//   len - length of array
//

void bubble_sort(int *a, int len) {
	int i, swap, done;
	do {
		done = 1;
		for (i = 1; i < len; i++){
			if (a[i-1] > a[i]) {
				swap = a[i-1];
				a[i-1] = a[i];
				a[i] = swap;
				done = 0;
			}
		}
	} while (!done);
	
}


// 
// name: Quick Sort - performs the quick sort algorithm on an array
//          of intergers
// @param
//   a - a pointer to an array to be sorted
//   len - length of array
//

void quick_sort(int *a, int left, int right) {
	int i, pivot, val, save;
	if ( right > left ) {
		pivot = left;
		val = a[pivot];
		swap(&a[pivot], &a[right]);
		save = left;
		for (i = left; i < right; i++){
			if ( a[i] < val ) {
				swap(&a[i],&a[save]);
				save++;
			}
		}
		swap(&a[save], &a[right]);
		pivot = save;
		quick_sort(a,left, pivot - 1);
		quick_sort(a, pivot + 1, right);
	}
}


// 
// name: Merge Sort - performs the merge sort algorithm on an array
//          of intergers
// @param
//   a - a pointer to an array to be sorted
//   len - length of array
//


void merge(int *C, int *A, int n, int *B, int m) {
	int i, j, k;
	for (i = 0, j = 0, k = 0; k < n + m; k++) {
		if (i == n) { C[k] = B[j++]; continue; }
		if (j == m) { C[k] = A[i++]; continue; }
		if (A[i] < B[j]) { C[k] = A[i++]; }
			else { C[k] = B[j++]; }
	}
}

void m_sort(int *A, int *B, int left, int right) {
	int mid;
	if (right > left) {
		mid = (right + left) / 2;
		m_sort(B, A, left, mid);
		m_sort(B, A, mid+1, right);
		merge(A+left, B+left, mid-left+1, B+mid+1, right-mid);
	}
}

void merge_sort(int *a, int left, int right) {
	int i, T[right];
	for (i = 0; i < right; i++) T[i] = a[i];
	m_sort(a, T, 0, right-1);	
}

// prints array of elements in rows of 10
int print_array(int *a, int len) {
	int i;
	
	printf("[\n");
	for(i = 1; i < len+1; i++) {
		printf(" %4d,",a[i-1]);
		if ( !(i % 10) ) {
			printf("\n");
		}
	}
	printf("]\n\n");
	return 0;
}

// creates an array of size len with random numbers 1-len
int fill_array(int *a, int len) {
	int i;
	
	for (i = 0; i < len; i++){
		a[i] = rand() % len + 1;
	}
	return 0;
}

// compare fuction provides function necessary for qsort
int compare ( const void *a, const void *b) {
	return ( *(int*)a - *(int*)b );
}


int main(int argc, const char* argv[]) {
	int sort_choice, len, *array, start, finish;
	struct timeval tm_begin, tm_end;
	
	// check for the proper number of arguments
	if (argc != 3) {
		fprintf(stderr, "Syntax:\n\t%s <sort algorithm> <problem size>\n", argv[0]);
		return -1;
	}
	sort_choice = atoi(argv[1]);		// which sort algorithm to execute
	len = atoi(argv[2]);				// length of array
	
	srand( time(NULL) );				// initialize rand based on system time
	
	array = ( int* ) malloc((len)* sizeof(int));
	fill_array(array, len);
//	print_array(array, len);
	
	gettimeofday(&tm_begin, NULL);		// store start time
	switch (sort_choice) {
		case 1:
			insertion_sort(array,len);
			break;
		case 2:
			bubble_sort(array, len);
			break;
		case 3:
			quick_sort(array, 0, len-1);
			break;
		case 4:
			merge_sort(array, 0, len);
			break;
		case 5:
			qsort(array, len, sizeof(int), compare);
			break;
		default:
			printf("\tInvalid sort alogithm selected\n");
			return -1;
	}
	gettimeofday(&tm_end, NULL);		// store end time
//	print_array(array, len);
	free(array);
	start = (tm_begin.tv_usec + tm_begin.tv_sec*1000000);	
	finish = (tm_end.tv_usec + tm_end.tv_sec*1000000);
	// print results in formated output
	printf("%d, %d, %d\n",sort_choice, len, finish-start);
	return 0;
}

