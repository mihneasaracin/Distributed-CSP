#include <iostream>
#include <mpi.h>
#include "include/DistributedRuntime.h"

//Run with:  mpirun  --oversubscribe -np 2 1

void example1(){
    std::cout << "Run example1()" << std::endl;
    DistributedRuntime::DistributedChannelUnbounded<int> channel1(2), channel2(2);
    DistributedRuntime::run();
    if (MPIUtils::getCurrentProc() == 0) {
        int message1, message2;
        int ret1,ret2;

        ret1= channel1.read(&message1, true);
        if (ret1){
            std::cout << "Message1 " << message1 << std::endl;
        }
        ret2 = channel2.read(&message2, true);
        if (ret2){
            std::cout << "Message2 " << message2 << std::endl;
        }



    } else if (MPIUtils::getCurrentProc() == 1){
        int message3 = 788, message4=333;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        channel1 <- message3;
        channel2 <- message4;
    }

}


// Ran with:  mpirun  --oversubscribe -np 2 2
void example2(){
    std::cout << "Run example2()" << std::endl;
    DistributedRuntime::DistributedChannelUnbounded<std::string> channel1(3);
    DistributedRuntime::run();
    if (MPIUtils::getCurrentProc() == 0) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        for(int i = 0 ; i < 4; ++i){
            std::string s;
            s <- channel1;
            std::cout << "GOT! " << s << std::endl;
        }

    } else {
        channel1.write("a");
        channel1.write("b");


    }

}

// Ran with:  mpirun  --oversubscribe -np 2 3
// should give error bc we write on a closed channel
void example3(){
    std::cout << "Run example3()" << std::endl;
    DistributedRuntime::DistributedChannelUnbounded<int> channel1(3);
    DistributedRuntime::DistributedChannelUnbounded<std::string> channel2(4);
    DistributedRuntime::run();
    if (MPIUtils::getCurrentProc() == 0) {
        channel1.write(2);
        channel1.close();
    } else {
        int val;
        channel1.read(&val);
        std::cout << "GOT" << val << std::endl;
        channel1.close();
        channel1.write(353);
    }

}


int main(int argc, char* argv[]) {
    if (std::string(argv[1]) == "1") {
        example1();
    } else if (std::string(argv[1]) == "2") {
        example2();
    } else if (std::string(argv[1]) == "3") {
        example3();
    }

    return 0;
}