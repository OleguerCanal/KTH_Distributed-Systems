#!/bin/bash -l

# time allocation
#SBATCH -A edu20.SF2568
# job name
#SBATCH -J covid_sim
# email notification
#SBATCH --mail-type=BEGIN,END
# 10 minutes wall-clock time will be given to this job
#SBATCH -t 05:00:00
# Number of nodes
#SBATCH --nodes=2
# set tasks per node to 24 in order to disablr hyperthreading
#SBATCH --ntasks-per-node=24

module add cmake/3.1.3
module add i-compilers intelmpi

rm -rf build/
mkdir build
cd build
cmake ..
make
cd ..

rm TIMES.txt
rm data/evolution.txt
rm data/hist_data.txt

WS_base=500

for i in 1 2 3
do
    for num in 1 5 10
    do
      	let "N=$num*$num*1000"
        let "WS=$num*$WS_base"

        # Baseline
        mpirun -n 1 build/my_mpi_bin $N $WS 1
        
        P=2    # Number of processors
        for p_x in 1 2
        do
            mpirun -n $P build/my_mpi_bin $N $WS $p_x
        done

        P=4    # Number of processors
        for p_x in 1 2 4
        do
            mpirun -n $P build/my_mpi_bin $N $WS $p_x
        done

        P=8    # Number of processors
        for p_x in 1 2 4 8
        do
            mpirun -n $P build/my_mpi_bin $N $WS $p_x
        done

        P=16    # Number of processors
        for p_x in 1 2 4 16
        do
            mpirun -n $P build/my_mpi_bin $N $WS $p_x
        done

	    P=24    # Number of processors
        for p_x in 1 24
        do
            mpirun -n $P build/my_mpi_bin $N $WS $p_x
        done

        P=32    # Number of processors
        for p_x in 1 32
        do
            mpirun -n $P build/my_mpi_bin $N $WS $p_x
        done

        P=48    # Number of processors
        for p_x in 1 48
        do
            mpirun -n $P build/my_mpi_bin $N $WS $p_x
        done
    done
done
