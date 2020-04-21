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


bool communicate(Region *region, std::default_random_engine *generator) {
    // Exhanges people with other regions
    std::list<Person> people_to_prev_region;
    std::list<Person> people_to_next_region;
    std::list<Person> people_to_above_region;
    std::list<Person> people_to_below_region;
    region->movePeople(generator, &people_to_prev_region, &people_to_next_region, &people_to_above_region, &people_to_below_region);
    std::vector<Person> incoming_people;
    exchange_people(*(region->coordinates), people_to_prev_region, people_to_next_region, people_to_above_region, people_to_below_region, &incoming_people);
    region->addPeople(incoming_people);
    return (incoming_people.size() > 0);
}

int main(int argc, char** argv) {
    int p, P;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    MPI_Comm_rank(MPI_COMM_WORLD, &p);

    std::default_random_engine generator(time(0) + p * 1000);
    if (P % env::processors_in_x_direction != 0) {
        MPI_Finalize();
        return -1;
    }
    env::RegionCoordinates region_coordinates(p, P);

    //todo: separate zone and create regions and people
    Region region(env::number_of_people, &region_coordinates, &generator);
    if (p == 0) {
        Person* Mike = region.getRandomPerson();
        Mike->getInfected(&generator);
    }

    std::stringstream msg;
    std::string Status = region.getStatus();
    std::cout.precision(3);
    msg << "p" << p << " t" << -1 << ": " << Status << std::endl;
    std::cout << msg.str();
    msg.str(""); 

    int iteration = 0;
    int vis_freq = (int) (0.1/env::TIME_STEP); // Update every day

    for (float t = 0; t <= env::nrDays; t += env::TIME_STEP) {
        bool change1 = communicate(&region, &generator);
        bool change2 = region.updateStatus(&generator);

        change1 = change1 | region.deleteSidePeople();

        if (change2)
            std::cout << "p: " << p << ", t:" << t << ", Status: " << region.getStatus() << std::endl;

        if (iteration%vis_freq == 0) {
            print_to_file(region, p, P);
        }
        iteration += 1;
    }

    Status = region.getStatus();
    msg << "Fp: " << p << ",        Status: " << Status << std::endl;
    std::cout << msg.str();
    msg.str("");
    std::cout << "iterations: " << iteration << std::endl;
    std::cout << p << ", " << P << std::endl;
    MPI_Finalize();
}