cmake_minimum_required(VERSION 3.12)
project(Distributed_CSP)

set(CMAKE_CXX_STANDARD 11)
file(GLOB source_files
        "src/*.cpp"
        "include/*.h"
        )

find_package(MPI REQUIRED)
set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS} ${MPI_LINK_FLAGS})
SET(CMAKE_C_COMPILER mpicc)
SET(CMAKE_CXX_COMPILER mpicxx)
include_directories(MPI_INCLUDE_PATH)
add_executable(main SelectTest_generated.cpp ${source_files})
target_link_libraries(main ${MPI_LIBRARIES})


# OMPI_CXX=g++-8 ~/Downloads/clion-2018.2.4/bin/cmake/linux/bin/cmake  --build ~/Documents/distributed_csp/Distributed-CSP --target main -- -j 4