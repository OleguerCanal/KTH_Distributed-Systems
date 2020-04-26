cd build
make
cd ..
rm evolution.txt
rm data/evolution.txt
rm data/hist_data.txt
mpirun -n 8 build/my_mpi_bin
# python visualize.py
python plot_hist.py
