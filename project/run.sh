cd build
make
cd ..
rm evolution.txt
rm data/evolution.txt
rm data/hist_data.txt

P=4											# Number of processors
p_x=2										# Number of processors in the x direction
N=1000										# Total number of people (whole world)
WS=500										# World size. Try to keep WS^2/N ~ 250

mpirun -n $P build/my_mpi_bin $N $WS $p_x	# Run the program

python plots/plot_hist.py					# Plot the histogram
python plots/visualize.py					# Plot the animation video