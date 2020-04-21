#include <iostream>
#include <fstream>
#include <string>
#include "mpi.h"
#include <person.hpp>
#include <region.hpp>


void print_to_file(Region region, int p, int P) {
    std::ofstream myfile;

    bool sendBuffer, recieveBuffer;
    MPI_Status status;
	if (p == 0) {  // First processor writes to file
        myfile.open("evolution.txt", std::ios_base::app | std::ios_base::out);
        // // std::cout << region.get_serialized_people() << std::endl;
        myfile << region.get_serialized_people();
        if (P == 1)
            myfile << "\n";
        myfile.close();
        if (P > 1)
		    MPI_Send(&sendBuffer,1,MPI::BOOL,1,0,MPI_COMM_WORLD); // Tell p1 to start writting
	}
	if (P > 1 && p > 0 && p != P-1) {
		MPI_Recv(&recieveBuffer,1,MPI::BOOL,p-1,0,MPI_COMM_WORLD, &status);
        myfile.open("evolution.txt", std::ios_base::app | std::ios_base::out);
        myfile << region.get_serialized_people();
        myfile.close();
		MPI_Send(&sendBuffer,1,MPI::BOOL,p+1,0,MPI_COMM_WORLD); // Tell the others to start writting
	}
	if (P > 1 && p == (P-1)) {
		MPI_Recv(&recieveBuffer,1,MPI::BOOL,p-1,0,MPI_COMM_WORLD, &status);
        myfile.open("evolution.txt", std::ios_base::app | std::ios_base::out);
        myfile << region.get_serialized_people();
        myfile << "\n";
        myfile.close();
    }
}

void send_people(int to, const std::list<Person> &people) {
    int size = 3*people.size();  // A person is defined by 3 floats
    float encoded_people[size];
    int i = 0;
    for (auto it = people.begin(); it != people.end(); ++it){
        encoded_people[i] = it->x;
        encoded_people[i+1] = it->y;
        encoded_people[i+2] = (float) it->status_;
        i += 3;
    }
    // std::cout << "sending size: " << ((float) size)/3.0 << ", to: " << to << std::endl;
    MPI_Send(&size,1,MPI::INT,to,0,MPI_COMM_WORLD); // Send number of people
    // std::cout << "sent size: " << ((float) size)/3.0 << ", to: " << to << std::endl;
    if (size == 0) return;
    MPI_Send(&encoded_people,size,MPI::FLOAT,to,0,MPI_COMM_WORLD); // Send encoded people
}

void receive_people(int from, std::vector<Person> *people) {
    MPI_Status status;
    int size;
    // std::cout << "receiving from: " << from << std::endl;
    MPI_Recv(&size,1,MPI::INT,from,0,MPI_COMM_WORLD, &status);
    // std::cout << "received size: " << ((float) size)/3.0 <<  ", from: " << from << std::endl;
    if (size == 0) return;
    float encoded_people[size];
    MPI_Recv(&encoded_people,size,MPI::FLOAT,from,0,MPI_COMM_WORLD, &status);
    people->reserve(size/3);
    for(int i = 0; i < size; i += 3) {
        Person person(encoded_people[i], encoded_people[i+1], (int) encoded_people[i+2]);
        people->push_back(person);
    }
}

void update_border_people_x(env::RegionCoordinates r_coord,
                            std::list<Person> *people_to_left_region,
                            std::list<Person> *people_to_right_region) {
    if (r_coord.px == 0) {
        for (Person& pers : *people_to_left_region)
            pers.x += env::world_size_;
    }
    if (r_coord.px == r_coord.Px-1) {
        for (Person& pers : *people_to_right_region)
            pers.x -= env::world_size_;
    }
}

void update_border_people_y(env::RegionCoordinates r_coord,
                            std::list<Person> *people_to_below_region,
                            std::list<Person> *people_to_above_region) {
    if (r_coord.py == 0) {
        for (Person& pers : *people_to_below_region)
            pers.y += env::world_size_;
    }
    if (r_coord.py == r_coord.Py - 1) {
        for (Person& pers : *people_to_above_region)
            pers.y -= env::world_size_;
    }
}

void exchange_people(env::RegionCoordinates r_coord,
                    std::list<Person> people_to_left_region,
                    std::list<Person> people_to_right_region,
                    std::list<Person> people_to_above_region,
                    std::list<Person> people_to_below_region,
                    std::vector<Person> *incoming_people) {
    // TODO Case P%2 = 1
    if (r_coord.P == 1) {
        update_border_people_x(r_coord, &people_to_left_region, &people_to_right_region);
        update_border_people_y(r_coord, &people_to_below_region, &people_to_above_region);
        for (Person& pers : people_to_left_region)  incoming_people->push_back(pers);
        for (Person& pers : people_to_right_region) incoming_people->push_back(pers);
        for (Person& pers : people_to_below_region) incoming_people->push_back(pers);
        for (Person& pers : people_to_above_region) incoming_people->push_back(pers);
        return;
    }

    // Update wolrd border people's coordinates
    update_border_people_x(r_coord, &people_to_left_region, &people_to_right_region);

    // Exchange 
    if (r_coord.color == "black") {
        // Exchange right
        send_people(r_coord.get_right_region_processor(), people_to_right_region);
        receive_people(r_coord.get_right_region_processor(), incoming_people);
        // Exchange left
        send_people(r_coord.get_left_region_processor(), people_to_left_region);
        receive_people(r_coord.get_left_region_processor(), incoming_people);
    }
    if (r_coord.color == "red") {
        // Exchange left
        receive_people(r_coord.get_left_region_processor(), incoming_people);
        send_people(r_coord.get_left_region_processor(), people_to_left_region);
        // Exchange right
        receive_people(r_coord.get_right_region_processor(), incoming_people);
        send_people(r_coord.get_right_region_processor(), people_to_right_region);   
    }

    for (Person& pers : *incoming_people) {
        if (pers.y < r_coord.bound.lower + env::infection_distance_)
            people_to_below_region.push_back(pers);
        if (pers.y > r_coord.bound.upper - env::infection_distance_)
            people_to_above_region.push_back(pers);
    }

    update_border_people_y(r_coord, &people_to_below_region, &people_to_above_region);

    // Exchange 
    if (r_coord.color == "black") {
        // Exchange up
        send_people(r_coord.get_above_region_processor(), people_to_above_region);
        receive_people(r_coord.get_above_region_processor(), incoming_people);
        // Exchange down
        send_people(r_coord.get_below_region_processor(), people_to_below_region);
        receive_people(r_coord.get_below_region_processor(), incoming_people);
    }
    else {
        // Exchange down
        receive_people(r_coord.get_below_region_processor(), incoming_people);
        send_people(r_coord.get_below_region_processor(), people_to_below_region);
        // Exchange up
        receive_people(r_coord.get_above_region_processor(), incoming_people);
        send_people(r_coord.get_above_region_processor(), people_to_above_region);
    }
}