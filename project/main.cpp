#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#include<person.hpp>
#include<region.hpp>


int main(int argc, char** argv) {
    int p, P;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    MPI_Comm_rank(MPI_COMM_WORLD, &p);
    MPI_Status status;

    std::cout << p << ", " << P << std::endl;
    MPI_Finalize();
}