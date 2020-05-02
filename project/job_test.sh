# Build
rm -rf build/
mkdir build
cd build
cmake ..
make
cd ..

WS_base=500 # World size. Try to keep WS^2/N ~ 100

for i in 1
do
    for num in 2
    do
        let "N=$num*$num*1000"
        let "WS=$num*$WS_base"

        # Baseline
        mpirun -n 1 build/my_mpi_bin $N $WS 1
        python plots/visualize.py
        python plots/plot_hist.py

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

        # P=8    # Number of processors
        # for p_x in 1 2 4 8
        # do
        #     mpirun -n $P build/my_mpi_bin $N $WS $p_x
        # done
    done
done