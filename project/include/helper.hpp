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
	if (p==0) {  // First processor writes to file
        myfile.open("evolution.txt", std::ios_base::app);
        // std::cout << region.get_serialized_people() << std::endl;
        myfile << region.get_serialized_people();
        myfile.close();
        if (P > 1)
		    MPI_Send(&sendBuffer,1,MPI::BOOL,1,0,MPI_COMM_WORLD); // Tell p1 to start writting
	}
	if (P > 1 && p%(P-1) > 0) {
		MPI_Recv(&recieveBuffer,1,MPI::BOOL,p-1,0,MPI_COMM_WORLD, &status);
        myfile.open("evolution.txt", std::ios_base::app);
        myfile << region.get_serialized_people();
        myfile.close();
		MPI_Send(&sendBuffer,1,MPI::BOOL,p+1,0,MPI_COMM_WORLD); // Tell the others to start writting
	}
	if (P > 1 && p == (P-1)) {
		MPI_Recv(&recieveBuffer,1,MPI::BOOL,p-1,0,MPI_COMM_WORLD, &status);
        myfile.open("evolution.txt", std::ios_base::app);
        myfile << region.get_serialized_people();
        myfile.close();
    }
    myfile.open("evolution.txt", std::ios_base::app);
    myfile << "\n";
    myfile.close();
}