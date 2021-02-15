#include <iostream>
#include <mpi.h>
#include "include/DistributedRuntime.h"






// Ran with:  mpirun  --oversubscribe -np 2 example1


int main(int argc, char* argv[]) {
    DistributedRuntime::DistributedChannelUnbounded<int> channel1(2), channel2(2);
    DistributedRuntime::run();
    if (MPIUtils::getCurrentProc() == 0) {
        int message1, message2;

        select {
                case message1 <- channel1:
                {
                    std::cout << "1" << std::endl;
                }
                case message2 <- channel2:
                {
                    std::cout << "2" << std::endl;
                }

        }


    } else {
        int message2 = 1;
        channel1 <- message2;
    }
    return 0;
}

