#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "mpi.h"

#include <person.hpp>
#include <region.hpp>
#include <env.hpp>


int main(int argc, char** argv) {
    int p, P;
    int nrDays = 50;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    MPI_Comm_rank(MPI_COMM_WORLD, &p);
    MPI_Status status;

    //todo: separate zone and create regions and people
    int number_of_people = 10;
    Region region(number_of_people, p, P);
    region.print();
    // for (float t = 0; t <= nrDays; t += env::TIME_STEP) {
    //     region.movePeople();
    //     //todo: send people to neighbours
    //     region.updateStatus();
    //     //todo: output
    // }

    std::cout << p << ", " << P << std::endl;
    MPI_Finalize();
}