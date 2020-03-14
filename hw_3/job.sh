#!/bin/bash -l

# time allocation
#SBATCH -A edu20.SF2568
# job name
#SBATCH -J myjob
# email notification
#SBATCH --mail-type=BEGIN,END
# 10 minutes wall-clock time will be given to this job
#SBATCH -t 00:10:00
# Number of nodes
#SBATCH --nodes=2
# set tasks per node to 24 in order to disablr hyperthreading
#SBATCH --ntasks-per-node=24

module add i-compilers intelmpi

mpiexec -n 1 odd_even_sort 100000000
mpiexec -n 2 odd_even_sort 100000000
mpiexec -n 4 odd_even_sort 100000000
mpiexec -n 5 odd_even_sort 100000000
mpiexec -n 8 odd_even_sort 100000000
mpiexec -n 10 odd_even_sort 100000000
mpiexec -n 16 odd_even_sort 100000000
mpiexec -n 20 odd_even_sort 100000000
