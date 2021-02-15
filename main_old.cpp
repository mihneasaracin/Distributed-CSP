#include <iostream>
#include <mpi.h>
#include "include/hps-1.0.0/src/hps.h"
#include <typeinfo>
#include <map>
#include <type_traits>
#include <utility>
#include "include/DistributedRuntime.h"
#include <functional>



//  OMPI_CXX=g++-8 ~/Downloads/clion-2018.2.4/bin/cmake/linux/bin/cmake --build ~/Documents/Dissertation/Distributed-CSP/cmake-build-debug --target main -- -j 4

using namespace std;

//typedef function<void()> func;
//
//
//void da(int dd){
//    cout << dd << "\n";
//
//}
//
//int da1(int e, char d, int * c){
//    return 1;
//}
//
//
//int nu(double d){
//    return 3;
//}
//
//void v(){
//
//}



int main(int argc, char* argv[]) {
//     map<string, func> anym;
//     auto f = bind(da, placeholders::_1);
//     f(1);
////     anym["da"] = f;
//     anym.emplace(make_pair("da", v));
//     anym["da"](3);



//    HashRing r;
//
//
//    r.addNode(0);
//    r.addNode(1);
//    r.addNode(2);
//    r.addNode(3);
//    r.addNode(4);
//    for (int i=0; i<100; ++i){
//        r.assignKeyToNode(to_string(i % 6));
//    }
//    r.assignKeyToNode(to_string(1));
//    r.assignKeyToNode(to_string(2));
//    r.assignKeyToNode(to_string(3));
//    r.assignKeyToNode(to_string(4));;


    DistributedRuntime::DistributedChannelUnbounded<int> channel1(4), channel2(2);// channel3, channel4, channel5, channel6;
    DistributedRuntime::DistributedChannelBounded<int> ch3;
    DistributedRuntime::run();
//    vector v1({1,1,2,3,3,4,32,65423,342});
//    vector v2({1});
//    vector v3({1});
//if (MPIUtils::getCurrentProc() == 0) {
//    ch3.write(1);
//
//}
    int val;
//    channel1.read(&val, false);
    channel1.close();
//    channel1.read(&val);
//    std::string serialized = hps::to_string(&channel1);
//    hps::from_string<DistributedRuntime::DistributedChannelUnbounded<int> >(serialized);
    cout << "CLOSEEE" << channel1.isClosed() << endl;

////
//if (MPIUtils::getCurrentProc() == 0) {
//
//
//    bool res1 = channel2.write(8989);
//    cout << MPIUtils::getCurrentProc() << "RESULTWRITE1= " << res1 << endl;
//
//}
//else{
//    int val;
//    std::this_thread::sleep_for(chrono::seconds(1));
//    cout << MPIUtils::getCurrentProc() << "BeforeRESULTREAD " << endl;
//
//    bool res2 = channel2.read(&val);
//    cout << MPIUtils::getCurrentProc() << "RESULTREAD1= " << val << endl;
////    channel2.read(&val);
//
//}

//   bool res2 = channel2.write(2);
//   cout << MPIUtils::getCurrentProc()<< "RESULT2= " << res2 << endl;
//   bool res3 = channel2.write(3, false);
//   cout << "RESULT3= " << res3 << endl;

//    }



////
//    vector<DistributedRuntime::DistributedChannelUnbounded<int>*> chs;
//    for(int i = 0; i <100 ; ++i){
//        chs.push_back(new DistributedRuntime::DistributedChannelUnbounded<int>());
//    }
////    DistributedRuntime::run();
//////    std::this_thread::sleep_for(chrono::seconds(10));
////
////    DistributedRuntime::DistributedChannelUnbounded<int> channel11(4, 1);
////    int val = 1, res;
////
//////    if (MPIUtils::getCurrentProc() >= 2){
//////        std::this_thread::sleep_for(chrono::seconds(2));
//////    }
//////    channel1.write(1);
//////    channel1.read(&res);
//////    cout << "Result = " << res << endl;
////
////
////////    MPI_Finalize();
//    for(int i = 0; i <100 ; ++i){
//        delete chs[i];
//    }


//    auto t_start = std::chrono::high_resolution_clock::now();
//    this_thread::sleep_for(chrono::seconds(2));
//    auto t_end = std::chrono::high_resolution_clock::now();
//
//    double elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end-t_start).count();
//
//    cout << "ELAPSED MILI " << elapsed_time_ms << endl;
//    cout << "ELAPSED SEC " << std::chrono::duration_cast<std::chrono::duration<float>>(t_end-t_start).count() << endl;






    //    DistributedChannelUnBounded<int> channel2;
//    DistributedChannelUnBounded<int> channel1;
//    vector<DistributedChannelUnBounded<int> > dd;
//    HashRing h(1);
//    for(int i= 0 ; i <= 6 ; ++i){
//        h.addNode(i);
//    }
//
//
//
//    string d = "2";
//    cout << endl;
//    cout << "DATA "<< h.fnv1(d.data()) << endl;
//    cout << h.assignKeyToNode(d) << endl;
//    h.deleteNode(3);
//
////    cout << "TEST  "<< typeid(channel2).name() << endl;
//    MPI_Finalize();
//    cout << GET_VARIABLE_NAME(channel2);


// serialization
//    std::vector<int> data({22, 333, -4444});
////
//    std::string serialized = hps::to_string(data);
//
////    cout << serialized;
//
//    auto parsed = hps::from_char_array<vector<int>>(serialized.data());
//    auto parsed1 = hps::from_string<vector<int> >(serialized);
//    for (int i=0; i < parsed.size(); i++) {
//        cout << parsed[i] << "\n";
//    }

}

//void example4(){
//    std::cout << "Run example4()" << std::endl;
//    DistributedRuntime::DistributedChannelUnbounded<int> channel1, channel2;
//    DistributedRuntime::run();
//    if (MPIUtils::getCurrentProc() == 0) {
//        int message1, message2;
//        select {
//                case message1 <- channel1:
//                {
//                    std::cout << "1" << std::endl;
//                }
//                case message2 <- channel2:
//                {
//                    std::cout << "2" << std::endl;
//                }
//        }
//    } else {
//        channel1.write(11);
//        channel2.write(22);
//
//    }
//
//}