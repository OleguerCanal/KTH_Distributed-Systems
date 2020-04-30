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

bool DEBUG = true;  // Turn true if you wanna print and save histogram data (SLOWER)

namespace env {
    float WORLD_SIZE = 10.0;
    int PROCESSORS_IN_X_DIRECTION = 2;
    int NR_PEOPLE = 5000; // per region
}

void printStatus(Region &region, float t) {
    if (!DEBUG) return;
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
    if (argc != 3) {
        printf("Wrong number of arguments");
        return 0;
    }

    int p, P;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    MPI_Comm_rank(MPI_COMM_WORLD, &p);
    std::cout.precision(6);
    double start_time = MPI_Wtime();

    env::PROCESSORS_IN_X_DIRECTION = std::min(P, 2);
    env::NR_PEOPLE = atoi(argv[1])/P;
    env::WORLD_SIZE = (float) atoi(argv[2]);

    if (p == 0 )
        std::cout << "P:" << P << ", n:" << env::NR_PEOPLE
        << ", N:" << P*env::NR_PEOPLE
        << ", WS:" << env::WORLD_SIZE << ", IR:" << env::INFECTION_RATE << std::endl; // Infection rate per time step

    std::default_random_engine generator(time(0) + p * 1000);
    // std::default_random_engine generator(2+p);
    if (P % env::PROCESSORS_IN_X_DIRECTION != 0) {
        MPI_Finalize();
        return -1;
    }
    env::RegionCoordinates region_coordinates(p, P);

    Region region(env::NR_PEOPLE, &region_coordinates, &generator);
    if (p == 0) {
        Person* Mike = region.getRandomPerson();
        Mike->getInfected(&generator);
    }

    printStatus(region, -1);
    int iteration = 0;
    int vis_freq = (int) (0.1/env::TIME_STEP);
    for (float t = 0; t <= env::NR_DAYS; t += env::TIME_STEP) {
        bool change = communicate(&region, &generator);

        if (DEBUG) {
            if (change)
                printStatus(region, t);

            if (iteration%vis_freq == 0)
                save_people_pos(region, p, P);
                // save_hist_data(region, p, P);
            iteration += 1;
        }
    }
    printStatus(region, -1);
    // MPI_Barrier(MPI_COMM_WORLD);
    if (p == 0) {
        double exec_time = MPI_Wtime() - start_time;
        std::cout << "The process took " << exec_time << " seconds to run." << std::endl;
        std::ofstream myfile;
        myfile.open("TIMES.txt", std::ios_base::app | std::ios_base::out);
        // P, TotalNumPeople, WorldSize
        myfile << P << "," << argv[1] << "," << argv[2] << "," << exec_time << "\n";
        myfile.close();
    }

    MPI_Finalize();
}