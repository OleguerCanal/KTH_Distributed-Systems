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
    for N in 50 500 1000 3000 6000
    do
        for WS in 1 4 8 16
        do
            mpirun -n $P build/my_mpi_bin $N $WS
        done
    done
done