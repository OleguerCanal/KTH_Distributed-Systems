#!/bin/bash -l

# time allocation
#SBATCH -A edu20.SF2568
# job name
#SBATCH -J covid_sim
# email notification
#SBATCH --mail-type=BEGIN,END
# 10 minutes wall-clock time will be given to this job
#SBATCH -t 00:15:00
# Number of nodes
#SBATCH --nodes=1
# set tasks per node to 24 in order to disablr hyperthreading
#SBATCH --ntasks-per-node=24


module add i-compilers intelmpi

rm -rf build/
mkdir build
cd build
cmake ..
make
cd ..

WS_base=500

for i in 1
do
    for num in 1 2
    do
        let "N=$num*$num*1000"
        let "WS=$num*$WS_base"

        # Baseline
        mpirun -n 1 build/my_mpi_bin $N $WS 1

        # P=2    # Number of processors
        # for p_x in 1 2
        # do
        #     mpirun -n $P build/my_mpi_bin $N $WS $p_x
        # done

        # P=4    # Number of processors
        # for p_x in 1 2 4
        # do
        #     mpirun -n $P build/my_mpi_bin $N $WS $p_x
        # done

        P=16    # Number of processors
        for p_x in 1 2 4 8 16
        do
            mpirun -n $P build/my_mpi_bin $N $WS $p_x
        done
    done
done