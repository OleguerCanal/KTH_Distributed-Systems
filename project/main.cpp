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


void printStatus(Region &region, float t) {
    std::stringstream msg;
    msg << "p: " << region.coordinates->p << ", t:" << t << ", Status: " << region.getStatus() << std::endl;
    std::cout << msg.str();
}

bool communicate(Region *region, std::default_random_engine *generator) {
    // Exhanges people with other regions
    std::list<Person> people_to_prev_region;
    std::list<Person> people_to_next_region;
    std::list<Person> people_to_above_region;
    std::list<Person> people_to_below_region;
    std::list<Person> people_to_prev_region_infectious;
    std::list<Person> people_to_next_region_infectious;
    std::list<Person> people_to_above_region_infectious;
    std::list<Person> people_to_below_region_infectious;
    std::vector<Person> immigrant_people;
    std::vector<Person> border_people;
    region->movePeople(generator, &people_to_prev_region_infectious, &people_to_next_region_infectious, &people_to_above_region_infectious, &people_to_below_region_infectious, &people_to_prev_region, &people_to_next_region, &people_to_above_region, &people_to_below_region, &border_people);
    exchange_people(*(region->coordinates), people_to_prev_region_infectious, people_to_next_region_infectious, people_to_above_region_infectious, people_to_below_region_infectious, people_to_prev_region, people_to_next_region, people_to_above_region, people_to_below_region, &immigrant_people, &border_people);
    std::sort(border_people.begin(), border_people.end());
    region->addPeople(immigrant_people);
    bool change = region->updateStatus(generator,&border_people);
    return change;
}

int main(int argc, char** argv) {
    int p, P;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    MPI_Comm_rank(MPI_COMM_WORLD, &p);
    std::cout.precision(6);

    // std::default_random_engine generator(time(0) + p * 1000);
    std::default_random_engine generator(1);
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

    printStatus(region, -1);

    int iteration = 0;
    int vis_freq = (int) (0.1/env::TIME_STEP); // Update every day
    for (float t = 0; t <= env::nrDays; t += env::TIME_STEP) {

        int n_people = region.people_.size();
        bool change = communicate(&region, &generator);

        //if (n_people != region.people_.size()) {
        //    std::cout << "ERROR at t=" << t << std::endl;
        //    break;
        //}

        if (change || ((int) (t*10000))%10000==0)
            printStatus(region, t);
        
        if (iteration%vis_freq == 0)
            print_to_file(region, p, P);
        iteration += 1;
    }
    printStatus(region, -1);
    std::cout << "iterations: " << iteration << std::endl;
    std::cout << p << ", " << P << std::endl;
    MPI_Finalize();
}