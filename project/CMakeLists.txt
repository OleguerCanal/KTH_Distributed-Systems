
cmake_minimum_required(VERSION 3.10)

find_package(MPI REQUIRED)

include_directories(include)
add_executable(my_mpi_bin main.cpp src/person.cpp src/region.cpp)

target_include_directories(my_mpi_bin PRIVATE ${MPI_CXX_INCLUDE_PATH} person.hpp, region.hpp)
target_compile_options(my_mpi_bin PRIVATE ${MPI_CXX_COMPILE_FLAGS})
target_link_libraries(my_mpi_bin ${MPI_CXX_LIBRARIES} ${MPI_CXX_LINK_FLAGS})
