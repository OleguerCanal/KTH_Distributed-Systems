#include <fstream>
#include <iostream>
#include <person.hpp>
#include <region.hpp>
#include <string>

#include "mpi.h"

// Save for video plot
void save_people_pos(Region region, int p, int P) {
    std::ofstream myfile;
    bool sendBuffer, recieveBuffer;
    MPI_Status status;
    if (p == 0) {  // First processor writes to file
        myfile.open("data/evolution.txt", std::ios_base::app | std::ios_base::out);
        myfile << region.get_serialized_people();
        if (P == 1)
            myfile << "\n";
        myfile.close();
        if (P > 1)
            MPI_Send(&sendBuffer, 1, MPI::BOOL, 1, 0, MPI_COMM_WORLD);  // Tell p1 to start writting
    }
    if (P > 1 && p > 0 && p != P - 1) {
        MPI_Recv(&recieveBuffer, 1, MPI::BOOL, p - 1, 0, MPI_COMM_WORLD, &status);
        myfile.open("data/evolution.txt", std::ios_base::app | std::ios_base::out);
        myfile << region.get_serialized_people();
        myfile.close();
        MPI_Send(&sendBuffer, 1, MPI::BOOL, p + 1, 0, MPI_COMM_WORLD);  // Tell the others to start writting
    }
    if (P > 1 && p == (P - 1)) {
        MPI_Recv(&recieveBuffer, 1, MPI::BOOL, p - 1, 0, MPI_COMM_WORLD, &status);
        myfile.open("data/evolution.txt", std::ios_base::app | std::ios_base::out);
        myfile << region.get_serialized_people();
        myfile << "\n";
        myfile.close();
    }
}

// Save for histogram plot
void save_hist_data(Region region, int p, int P) {
    std::ofstream myfile;
    bool sendBuffer, recieveBuffer;
    MPI_Status status;
    if (p == 0) {  // First processor writes to file
        myfile.open("data/hist_data.txt", std::ios_base::app | std::ios_base::out);
        // // std::cout << region.get_serialized_people() << std::endl;
        myfile << region.getStatus(false);
        if (P == 1)
            myfile << "\n";
        myfile.close();
        if (P > 1)
            MPI_Send(&sendBuffer, 1, MPI::BOOL, 1, 0, MPI_COMM_WORLD);  // Tell p1 to start writting
    }
    if (P > 1 && p > 0 && p != P - 1) {
        MPI_Recv(&recieveBuffer, 1, MPI::BOOL, p - 1, 0, MPI_COMM_WORLD, &status);
        myfile.open("data/hist_data.txt", std::ios_base::app | std::ios_base::out);
        myfile << region.getStatus(false);
        myfile.close();
        MPI_Send(&sendBuffer, 1, MPI::BOOL, p + 1, 0, MPI_COMM_WORLD);  // Tell the others to start writting
    }
    if (P > 1 && p == (P - 1)) {
        MPI_Recv(&recieveBuffer, 1, MPI::BOOL, p - 1, 0, MPI_COMM_WORLD, &status);
        myfile.open("data/hist_data.txt", std::ios_base::app | std::ios_base::out);
        myfile << region.getStatus(false);
        myfile << "\n";
        myfile.close();
    }
}

// Send the given 'people' to processor 'to'.
void send_people(int to, const std::list<Person>& people) {
    // Encode person class to 3 floats (x,y,status)
    int size = 3 * people.size(); 
    float encoded_people[size];
    int i = 0;
    for (auto it = people.begin(); it != people.end(); ++it) {
        encoded_people[i] = it->x;
        encoded_people[i + 1] = it->y;
        encoded_people[i + 2] = (float)it->status_;
        i += 3;
    }

    // First send how much you will send
    MPI_Send(&size, 1, MPI::INT, to, 0, MPI_COMM_WORLD);  // Send number of people
    if (size == 0) return;

    // Then send that number of people
    MPI_Send(&encoded_people, size, MPI::FLOAT, to, 0, MPI_COMM_WORLD);  // Send encoded people
}

void receive_people(int from, std::vector<Person>* people) {
    MPI_Status status;

    // Recieve the amount of people you will recieve
    int size;
    MPI_Recv(&size, 1, MPI::INT, from, 0, MPI_COMM_WORLD, &status);
    if (size == 0) return;

    // Recieve that amount of people
    float encoded_people[size];
    MPI_Recv(&encoded_people, size, MPI::FLOAT, from, 0, MPI_COMM_WORLD, &status);

    // Transform 3 floats into people class
    people->reserve(size / 3);
    for (int i = 0; i < size; i += 3) {
        Person person(encoded_people[i], encoded_people[i + 1], (int)encoded_people[i + 2]);
        people->push_back(person);
    }
}

// If a person arrives at an edge of the world, as the world wraps around,
// it should remove/add the world size from its coordinate.
void update_border_people_x(env::RegionCoordinates r_coord,
                            std::list<Person>* people_to_left_region,
                            std::list<Person>* people_to_right_region) {
    if (r_coord.px == 0) {
        for (Person& pers : *people_to_left_region)
            pers.x += env::WORLD_SIZE;
    }
    if (r_coord.px == r_coord.Px - 1) {
        for (Person& pers : *people_to_right_region)
            pers.x -= env::WORLD_SIZE;
    }
}

void update_border_people_y(env::RegionCoordinates r_coord,
                            std::list<Person>* people_to_below_region,
                            std::list<Person>* people_to_above_region) {
    if (r_coord.py == 0) {
        for (Person& pers : *people_to_below_region)
            pers.y += env::WORLD_SIZE;
    }
    if (r_coord.py == r_coord.Py - 1) {
        for (Person& pers : *people_to_above_region)
            pers.y -= env::WORLD_SIZE;
    }
}

// Does all the communication needed
void exchange_people(env::RegionCoordinates r_coord,
                    std::list<Person> people_to_left_region_infectious,
                    std::list<Person> people_to_right_region_infectious,
                    std::list<Person> people_to_above_region_infectious,
                    std::list<Person> people_to_below_region_infectious,
                    std::list<Person> people_to_left_region,
                    std::list<Person> people_to_right_region,
                    std::list<Person> people_to_above_region,
                    std::list<Person> people_to_below_region,
                    std::vector<Person>* immigrant_people,
                    std::vector<Person>* border_people) {

    // If close to the edge of the world, brings to the opposite edge
    update_border_people_x(r_coord, &people_to_left_region, &people_to_right_region);
    update_border_people_x(r_coord, &people_to_left_region_infectious, &people_to_right_region_infectious);
    
    std::vector<Person> tempImmigrant_people;

    // If only one processor in the x direction, simply reinsert people leaving in the same processor
    if (r_coord.Px == 1) {
        for (Person pers : people_to_left_region) tempImmigrant_people.push_back(pers);
        for (Person pers : people_to_right_region) tempImmigrant_people.push_back(pers);
        for (Person pers : people_to_left_region_infectious) border_people->push_back(pers);
        for (Person pers : people_to_right_region_infectious) border_people->push_back(pers);
    }
    else {
        // Exchange
        if (r_coord.color == "black") {
            // Exchange right
            send_people(r_coord.get_right_region_processor(), people_to_right_region);
            send_people(r_coord.get_right_region_processor(), people_to_right_region_infectious);
            receive_people(r_coord.get_right_region_processor(), &tempImmigrant_people);
            receive_people(r_coord.get_right_region_processor(), border_people);
            // Exchange left
            send_people(r_coord.get_left_region_processor(), people_to_left_region);
            send_people(r_coord.get_left_region_processor(), people_to_left_region_infectious);
            receive_people(r_coord.get_left_region_processor(), &tempImmigrant_people);
            receive_people(r_coord.get_left_region_processor(), border_people);
        }
        if (r_coord.color == "red") {
            // Exchange left
            receive_people(r_coord.get_left_region_processor(), &tempImmigrant_people);
            receive_people(r_coord.get_left_region_processor(), border_people);
            send_people(r_coord.get_left_region_processor(), people_to_left_region);
            send_people(r_coord.get_left_region_processor(), people_to_left_region_infectious);
            // Exchange right
            receive_people(r_coord.get_right_region_processor(), &tempImmigrant_people);
            receive_people(r_coord.get_right_region_processor(), border_people);
            send_people(r_coord.get_right_region_processor(), people_to_right_region);
            send_people(r_coord.get_right_region_processor(), people_to_right_region_infectious);
        }
    }

    // Checks for people in the corner and decided whether they should be sent further
    for (Person& pers : tempImmigrant_people) {
        if (pers.y <= r_coord.bound.lower + env::INFECTION_DISTANCE)
            if (pers.y <= r_coord.bound.lower) {
                people_to_below_region.push_back(pers);
                if (pers.isInfected())
                    border_people->push_back(pers);
            }
            else {
                immigrant_people->push_back(pers);
                if (pers.isInfected())
                    people_to_below_region_infectious.push_back(pers);
            }
        else if (pers.y > r_coord.bound.upper - env::INFECTION_DISTANCE)
            if (pers.y > r_coord.bound.upper) {
                people_to_above_region.push_back(pers);
                if (pers.isInfected())
                    border_people->push_back(pers);
            }
            else {
                immigrant_people->push_back(pers);
                if (pers.isInfected())
                    people_to_above_region_infectious.push_back(pers);
            }
        else
            immigrant_people->push_back(pers);
    }

    // Same for sending vertically
    update_border_people_y(r_coord, &people_to_below_region, &people_to_above_region);
    update_border_people_y(r_coord, &people_to_below_region_infectious, &people_to_above_region_infectious);

    if (r_coord.Py == 1) {
        for (Person pers : people_to_above_region) immigrant_people->push_back(pers);
        for (Person pers : people_to_below_region) immigrant_people->push_back(pers);
        for (Person pers : people_to_above_region_infectious) border_people->push_back(pers);
        for (Person pers : people_to_below_region_infectious) border_people->push_back(pers);
    }
    else {
        // Exchange
        if (r_coord.color == "black") {
            // Exchange up
            send_people(r_coord.get_above_region_processor(), people_to_above_region);
            send_people(r_coord.get_above_region_processor(), people_to_above_region_infectious);
            receive_people(r_coord.get_above_region_processor(), immigrant_people);
            receive_people(r_coord.get_above_region_processor(), border_people);
            // Exchange down
            send_people(r_coord.get_below_region_processor(), people_to_below_region);
            send_people(r_coord.get_below_region_processor(), people_to_below_region_infectious);
            receive_people(r_coord.get_below_region_processor(), immigrant_people);
            receive_people(r_coord.get_below_region_processor(), border_people);
        }
        if (r_coord.color == "red") {
            // Exchange down
            receive_people(r_coord.get_below_region_processor(), immigrant_people);
            receive_people(r_coord.get_below_region_processor(), border_people);
            send_people(r_coord.get_below_region_processor(), people_to_below_region);
            send_people(r_coord.get_below_region_processor(), people_to_below_region_infectious);
            // Exchange up
            receive_people(r_coord.get_above_region_processor(), immigrant_people);
            receive_people(r_coord.get_above_region_processor(), border_people);
            send_people(r_coord.get_above_region_processor(), people_to_above_region);
            send_people(r_coord.get_above_region_processor(), people_to_above_region_infectious);
        }
    }
}