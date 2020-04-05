#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <random>
#include "mpi.h"

#include <person.hpp>
#include <region.hpp>
#include <env.hpp>
#include <helper.hpp>


int main(int argc, char** argv) {
    int p, P;
    int nrDays = 50;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    MPI_Comm_rank(MPI_COMM_WORLD, &p);
    MPI_Status status;

    std::default_random_engine generator(time(0) + p * 1000);

    //todo: separate zone and create regions and people
    int number_of_people = 10;
    Region region(number_of_people, p, P, &generator);
    if (p == 0) {
        Person* Mike = region.getRandomPerson();
        Mike->getInfected(&generator);
    }
    //region.print();

    std::stringstream msg;
    std::string Status = region.getStatus();
    std::cout.precision(3);
    msg << "p" << p << " t" << -1 << ": " << Status << std::endl;
    std::cout << msg.str();
    msg.str(""); 

    int iteration = 0;
    int vis_freq = (int) (0.1/env::TIME_STEP); // Update every day
    std::cout << vis_freq << std::endl;
    for (float t = 0; t <= nrDays; t += env::TIME_STEP) {
        region.movePeople(&generator);
        //TODO: make sure people stay within borders, communication
        bool change = region.updateStatus(&generator);
        
        if (change) {
            std::string Status = region.getStatus();
            msg << "p: " << p << ", t:" << t << ", Status: " << Status << std::endl;
            std::cout << msg.str();
            msg.str("");
        }
        if (iteration%vis_freq == 0) {
            print_to_file(region, p, P);
        }
        iteration += 1;
    }
    std::cout << "iterations: " << iteration << std::endl;
    std::cout << p << ", " << P << std::endl;
    MPI_Finalize();
}