cd build
make
cd ..
rm evolution.txt
rm data/evolution.txt
rm data/hist_data.txt

P=4    # Number of processors
N=2000  # Total number of people (whole world)
WS=500 # World size. Try to keep WS^2/N ~ 100
mpirun -n $P build/my_mpi_bin $N $WS
# python plots/visualize.py
python plots/plot_hist.py

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