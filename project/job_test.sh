# Build
rm -rf build/
mkdir build
cd build
cmake ..
make
cd ..

# P=8    # Number of processors
# N=10  # Total number of people (whole world)
# WS=1   # World size
# mpirun -n $P build/my_mpi_bin $N $WS

for P in 1 2 4 8
do
    for N in 3000
    do
        for WS in 1 2 4
        do
            mpirun -n $P build/my_mpi_bin $N $WS
        done
    done
done