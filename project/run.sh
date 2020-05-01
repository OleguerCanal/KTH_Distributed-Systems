cd build
make
cd ..
rm evolution.txt
rm data/evolution.txt
rm data/hist_data.txt

P=8    # Number of processors
p_x=2
N=1000  # Total number of people (whole world)
WS=500 # World size. Try to keep WS^2/N ~ 100
mpirun -n $P build/my_mpi_bin $N $WS $p_x
python plots/visualize.py
python plots/plot_hist.py

# for p_x in 8 4 2 1
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