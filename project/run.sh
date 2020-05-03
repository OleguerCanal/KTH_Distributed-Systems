cd build
make
cd ..
rm evolution.txt
rm data/evolution.txt
rm data/hist_data.txt

# P=4    # Number of processors
# p_x=2
# # N=32000  # Total number of people (whole world)
# WS=500 # World size. Try to keep WS^2/N ~ 100
# mpirun -n $P build/my_mpi_bin $N $WS $p_x
# python plots/visualize.py
# python plots/plot_hist.py

# for p_x in 8 4 2 1
# do
#     mpirun -n $P build/my_mpi_bin $N $WS $p_x
# done

# for N in 1000 2000 4000 8000 16000 32000
# do
#     mpirun -n $P build/my_mpi_bin $N $WS $p_x
# done

# for P in 1 4 8
# do
#     for N in 100 500
#     do
#         for WS in 1 2 4
#         do
#             mpirun -n $P build/my_mpi_bin $N $WS
#         done
#     done
# done

# module add i-compilers intelmpi


WS_base=500

for i in 1
do
    for num in 1
    do
        let "N=$num*$num*1000"
        let "WS=$num*$WS_base"

        P=16    # Number of processors
        for p_x in 1 2 4 8 16
        do
            mpirun -n $P build/my_mpi_bin $N $WS $p_x
        done
    done
done