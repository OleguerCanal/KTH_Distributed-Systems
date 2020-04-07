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
        if (P == 0)
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

void exchange_people(int p, int P,
                    std::list<Person> people_to_prev_region,
                    std::list<Person> people_to_next_region,
                    std::vector<Person> *incoming_people) {
    if (P == 1) return;
    // TODO Case P%2 = 1

    if (p == 0) {
        for (Person& pers : people_to_prev_region)
            pers.x += P * env::world_size_;
    }
    if (p == P-1) {
        for (Person& pers : people_to_next_region)
            pers.x -= P * env::world_size_;
    }
    // Exchange 
    if (p%2 == 0) {
        // Exchange next
        send_people((p+1)%P, people_to_next_region);
        receive_people((p+1)%P, incoming_people);
        // std::cout << p << ": first step ok" << std::endl;
        // Exchange previous
        send_people((p+P-1)%P, people_to_prev_region);  // I add P to make it cyclic
        receive_people((p+P-1)%P, incoming_people);
    } else {
        // Exchange previous
        receive_people((p+P-1)%P, incoming_people);
        send_people((p+P-1)%P, people_to_prev_region);
        // std::cout << p << ": first step ok" << std::endl;
        // Exchange next
        receive_people((p+1)%P, incoming_people);
        send_people((p+1)%P, people_to_next_region);
    }
    // std::cout << "done" << std::endl;
}