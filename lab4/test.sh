#!/bin/bash
## This bash script executes my_sort and op_sort 100 times for 2^4 - 2^12
##
## Author  : Bundt, Josh
## Class   : CS2140 
## Project : Lab 4
## Date    : 03 August 2011

for sort in my_sort op_sort
do
  for b in 16 32 64 128 256 512 1024 2048 4096
  do
    for a in 1 2 3 4 5
    do
      for c in {1..100}
      do
        ./$sort $a $b >> "$sort"_test.csv
      done
    done
  done
done
