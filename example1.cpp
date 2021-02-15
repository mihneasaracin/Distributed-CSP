#include <iostream>
#include <mpi.h>
#include "include/DistributedRuntime.h"






using namespace std;



// Ran with:  mpirun  --oversubscribe -np 2 example1


int main(int argc, char* argv[]) {
    DistributedRuntime::DistributedChannelUnbounded<int> channel1(2);
    DistributedRuntime::run();
    if (MPIUtils::getCurrentProc() == 0) {
        int val;


    } else {

    }
    return 0;

}



}
