#include <iostream>
#include <mpi.h>
#include <stdlib.h>
#include <thread>

using namespace std;

// void ParallelMPITask(int a, string c){
//    MPI_Init(NULL, NULL);

//    int rank;
//    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
// }


template <typename Function, typename ...Ts>
void foo(Function func, Ts&&... args){
    

    func(args...);

}


void f(int a, int b, int c){
    cout <<" TT= " << a + b + c << endl;
}

void b(string c) {
    cout << "C+=" << c <<endl;
}


int main(int argc, char* argv[]) {
    int rank = 1;
    {
        int a = 1;
        foo(&f, a, 2 ,3);
    }

    foo(&b, "DA");



    // int c = 3;
    // string d = "a";
    /*
    *
    *
    *
    * a lot of code only on master
    *
    */
    int a = 3;
    string d = "a";

    //create a new file with these calls for the worker nodes
    //check function definition with test.py
    //####ParallelMPITask(f, )####

    // ParallelMPITask

    if (rank > 0) {
        // goto L2;


    }
    // parallel_mpi_task();
    // L2: cout <<'n';


}