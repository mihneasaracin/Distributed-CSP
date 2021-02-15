#include <iostream>
#include <mpi.h>
#include "include/DistributedRuntime.h"




// Ran with:  mpirun  --oversubscribe -np 2 example1


int main(int argc, char* argv[]) {
    DistributedRuntime::DistributedChannelUnbounded<int> channel1(2);
    DistributedRuntime::run();
    if(MPIUtils::getCurrentProc() == 0){
        int val;

        select{
                case message1 <- channel1:
                {
                    printf("1");
                }

        }


    } else{
        int msg1 = 1;
        channel1.write(msg);
    }
    return 0;

}
