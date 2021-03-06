#include <iostream>
#include <mpi.h>
#include "include/DistributedRuntime.h"




// Ran with:  mpirun  --oversubscribe -np 2 example1


int main(int argc, char* argv[]) {
    DistributedRuntime::DistributedChannelUnbounded<int> channel1(2);
    DistributedRuntime::run();
    if(MPIUtils::getCurrentProc() == 0){
        int message2;

        select {
                case message1 <- channel1:
                {
                    printf("1");
                }
            default:
                {
                    print("Nothing");
                }

        }


    } else{
        int message1 = 1;
        channel1 <- message1;
    }
    return 0;

}
