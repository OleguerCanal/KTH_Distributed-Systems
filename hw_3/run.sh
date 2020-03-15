#!/bin/bash -l
mpicc -std=c99 odd_even_sort.c -O3 -o odd_even_sort;

# mpiexec -n 3 odd_even_sort 10
# mpirun -np 3 ./odd_even_sort 1000000

for data_size in 1000000 10000000
do
    for (( nodes=1; nodes<=8; nodes++ )); do
        echo "Data: " $data_size ", Nodes: " $nodes
        mpirun -np $nodes ./odd_even_sort $data_size
    done
done