#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <list>
#include <random>
#include "mpi.h"

#include <person.hpp>
#include <region.hpp>
#include <env.hpp>
#include <helper.hpp>


int main(int argc, char** argv) {
    int p, P;
    int nrDays = 10;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    MPI_Comm_rank(MPI_COMM_WORLD, &p);
    MPI_Status status;

    std::default_random_engine generator(time(0) + p * 1000);

    //todo: separate zone and create regions and people
    int number_of_people = 100;
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
        std::list<Person> people_to_prev_region;
        std::list<Person> people_to_next_region;
        region.movePeople(&generator, &people_to_prev_region, &people_to_next_region);
        //std::cout << "p: " << p << ", prev: " << people_to_prev_region.size() << std::endl;
        //std::cout << "p: " << p << ", next: " << people_to_next_region.size() << std::endl;

        std::vector<Person> incoming_people;
        exchange_people(p, P, people_to_prev_region, people_to_next_region, &incoming_people);

        region.addPeople(incoming_people);
        people_to_prev_region.clear();
        people_to_next_region.clear();
        incoming_people.clear();

        bool change = region.updateStatus(&generator);
        region.deleteSidePeople();
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