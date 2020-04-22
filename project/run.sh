cd build
make
cd ..
rm evolution.txt
mpirun -n 4 build/my_mpi_bin
python visualize.py
