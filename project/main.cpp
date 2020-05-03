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
    float WORLD_SIZE;
    int PROCESSORS_IN_X_DIRECTION;
    int NR_PEOPLE; // per region
}

void printStatus(Region &region, float t) {
    if (!DEBUG) return;
    std::stringstream msg;
    msg << "p: " << region.coordinates->p << ", t:" << t << ", Status: " << region.getStatus() << std::endl;
    std::cout << msg.str();
}

bool simulateTimestep(Region *region, std::default_random_engine *generator) {
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

    // Move people and decide who goes where
    region->movePeople(generator, &people_to_prev_region_infectious, &people_to_next_region_infectious, &people_to_above_region_infectious, &people_to_below_region_infectious, &people_to_prev_region, &people_to_next_region, &people_to_above_region, &people_to_below_region, &border_people);
    
    // Exchange people with neighbouring processors
    exchange_people(*(region->coordinates), people_to_prev_region_infectious, people_to_next_region_infectious, people_to_above_region_infectious, people_to_below_region_infectious, people_to_prev_region, people_to_next_region, people_to_above_region, people_to_below_region, &immigrant_people, &border_people);
    
    // People at the border need to be sorted
    std::sort(border_people.begin(), border_people.end());

    // Immigrant people (incoming people) are added to the stored people and this gets resorted
    region->addPeople(immigrant_people);

    // The status of all people in the region gets updated (becomes infected or removed)
    bool change = region->updateStatus(generator,&border_people);
    return change;
}

int main(int argc, char** argv) {
    if (argc != 4) {
        printf("Wrong number of arguments");
        return 0;
    }

    // Start MPI
    int p, P;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    MPI_Comm_rank(MPI_COMM_WORLD, &p);
    std::cout.precision(6);
    double start_time = MPI_Wtime();

    // Input command line arguments
    env::PROCESSORS_IN_X_DIRECTION = atoi(argv[3]);
    env::NR_PEOPLE = atoi(argv[1])/P;
    env::WORLD_SIZE = (float) atoi(argv[2]);

    // Print status
    if (p == 0 )
        std::cout << "P:" << P << ", n:" << env::NR_PEOPLE
        << ", N:" << P*env::NR_PEOPLE
        << ", WS:" << env::WORLD_SIZE
        << ", IR:" << env::INFECTION_RATE << std::endl; // Infection rate per time step

    // std::default_random_engine generator(time(0) + p);
    std::default_random_engine generator(time(0) + p);

    // Check number of processors devidable by processors in x direction
    if (P % env::PROCESSORS_IN_X_DIRECTION != 0) {
        MPI_Finalize();
        return -1;
    }

    // Define region and create it with the right number of people
    env::RegionCoordinates region_coordinates(p, P);
    Region region(env::NR_PEOPLE, &region_coordinates, &generator);

    // Infect one person
    if (p == 0) {
        Person* Mike = region.getRandomPerson();
        Mike->getInfected(&generator);
    }

    // Print status, init counter, define frequency of ploting
    printStatus(region, -1);
    int iteration = 0;
    int vis_freq = (int) (0.1/env::TIME_STEP);

    // Main loop over timesteps
    for (float t = 0; t <= env::NR_DAYS; t += env::TIME_STEP) {
        // Execute one timestep
        bool change = simulateTimestep(&region, &generator);

        // Save information and print status
        if (DEBUG) {
            if (change)
                printStatus(region, t);

            if (iteration%vis_freq == 0) {
                save_people_pos(region, p, P);
                save_hist_data(region, p, P);
            }
            iteration += 1;
        }
    }

    // Print information at the end
    printStatus(region, -1);
    if (p == 0) {
        double exec_time = MPI_Wtime() - start_time;
        std::cout << "The process took " << exec_time << " seconds to run." << std::endl;
        std::ofstream myfile;
        myfile.open("TIMES.txt", std::ios_base::app | std::ios_base::out);
        // P, TotalNumPeople, WorldSize
        myfile << P << "," << argv[3] << "," << argv[1] << "," << exec_time << "\n";
        myfile.close();
    }

    // Finish
    MPI_Finalize();
}