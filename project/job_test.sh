# Build
rm -rf build/
mkdir build
cd build
cmake ..
make
cd ..

WS=500 # World size. Try to keep WS^2/N ~ 100

for i in 1 2 3 4
do
    for N in 1000 2000 16000 32000 64000 128000 256000
    do
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
    done
done