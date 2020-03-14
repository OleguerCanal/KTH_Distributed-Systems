mpicc odd_even_sort.c -O3 -o odd_even_sort; mpiexec -n 1 odd_even_sort 1000000
mpicc odd_even_sort.c -O3 -o odd_even_sort; mpiexec -n 2 odd_even_sort 1000000
mpicc odd_even_sort.c -O3 -o odd_even_sort; mpiexec -n 8 odd_even_sort 1000000
# mpicc odd_even_sort.c -O3 -o odd_even_sort; mpiexec -n 3 odd_even_sort 1000000
# mpicc odd_even_sort.c -O3 -o odd_even_sort; mpiexec -n 4 odd_even_sort 1000000
# mpicc odd_even_sort.c -O3 -o odd_even_sort; mpiexec -n 5 odd_even_sort 1000000
# mpicc odd_even_sort.c -O3 -o odd_even_sort; mpiexec -n 6 odd_even_sort 1000000
# mpicc odd_even_sort.c -O3 -o odd_even_sort; mpiexec -n 7 odd_even_sort 1000000
