#!/bin/bash -l

# time allocation
#SBATCH -A edu20.SF2568
# job name
#SBATCH -J sorting
# email notification
#SBATCH --mail-type=BEGIN,END
# 10 minutes wall-clock time will be given to this job
#SBATCH -t 00:30:00
# Number of nodes
#SBATCH --nodes=2
# set tasks per node to 24 in order to disablr hyperthreading
#SBATCH --ntasks-per-node=24

module add i-compilers intelmpi

mpicc -std=c99 odd_even_sort.c -O3 -o odd_even_sort;

for data_size in 10000000 100000000 1000000000
do
    for (( nodes=1; nodes<=24; nodes++ )); do
        mpirun -np $nodes ./odd_even_sort $data_size
    done
done