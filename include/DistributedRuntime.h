#pragma once

#ifndef DISTRIBUTED_CSP_DISTRIBUTEDRUNTIME_H
#define DISTRIBUTED_CSP_DISTRIBUTEDRUNTIME_H


#include <mpi.h>
#include <thread>
#include <atomic>
#include <iostream>
#include "hps-1.0.0/src/hps.h"
#include "Utils.h"
#include "HashRing.h"
#include "ChannelUnbounded.h"


namespace DistributedRuntime{

    const std::string WRITE_PREFIX = "#write#";
    const std::string READ_PREFIX = "#read#";
    const std::string CLOSE_PREFIX = "#close#";
    const std::string DELETE_PREFIX = "#delete#";
    const int MSG_MAX_LENGTH = 10000;
    const double MSG_RECV_TIMEOUT_SEC = 1;
    MPI_Comm writeComm;
    MPI_Comm readComm;
    void addChannel(int, int);
    int assignChannelToNode(int);
    void deleteChannel(int);
    void listenToMessages();
    void init();
    std::thread channelWorker;
    static std::atomic_bool finished;
    static int channelCount = 0;

    template <class T>
    class DistributedChannelUnbounded {

        protected:
            ChannelUnbounded<T>* localChannel;
            int channelID = -1;
            int maxsize;
            int currentProc;
            int assignedNode;
            bool m_isClosed;
            bool takeIntoAccount = false;
            virtual void init(int, int);
            virtual void deleteRemoteChannel();

        public:
            DistributedChannelUnbounded(int sz = 2, int id = -1);
            DistributedChannelUnbounded(const DistributedChannelUnbounded<T> &ch);
            ~DistributedChannelUnbounded();
            virtual bool isRemote();
            virtual void close();
            virtual bool isClosed();
            virtual bool write(const T& value, const bool wait = true);
            virtual bool read(T* value, bool wait = true);


            // these are for hps library to serialize but not working atm
//            template <class B>
//            void serialize(B& buf) const {
//                buf << channelCount << channelID << maxsize << currentProc << assignedNode << m_isClosed <<takeIntoAccount;
//            }
//
//            template <class B>
//            void parse(B& buf) {
//                buf >> channelCount >> channelID >> maxsize >> currentProc >> assignedNode >> m_isClosed >> takeIntoAccount;
//            }
    };

    template <class T>
    class DistributedChannelBounded: public DistributedChannelUnbounded <T> {
        public:
            DistributedChannelBounded(): DistributedChannelUnbounded <T>(1){}
    };

//    template <class T>
//    int DistributedChannelUnbounded<T>::channelCount = 0;


    template <class T>
    void DistributedChannelUnbounded<T>::init(int sz, int id){
        maxsize = sz;
        channelID = id;

        if (maxsize < 0){
            std::cerr << std::endl << "Error! Process" << currentProc << " tried to create a channel with size < 0!" << std::endl;
            MPI_Abort(MPI_COMM_WORLD, MPI_ERR_OTHER);
        }

        // if id already assigned(i.e id > 0 at the constructor call) it means that this is not a new 'distributed' channel
        if (channelID < 0){

            // Init environment if this is the first channel created
            if (channelCount == 0) {
                std::cout<< "CALLED DistributedRuntime::init()\n";
                DistributedRuntime::init();
            }
            takeIntoAccount = true;
            channelID = ++channelCount;
        }

        m_isClosed = false;
        currentProc = MPIUtils::getCurrentProc();

        assignedNode = DistributedRuntime::assignChannelToNode(channelID);
        if (!takeIntoAccount){
            localChannel = new ChannelUnbounded<T>(maxsize);
        }
//        std::cout << "Created channel with size = " << maxsize << " and id = " << channelID << " assignedNode= " << assignedNode << " "<< sz << " " << id << std::endl;

        if (takeIntoAccount && !isRemote()){
            addChannel(channelID, maxsize);
        }
    }

    template <class T>
    DistributedChannelUnbounded<T>::DistributedChannelUnbounded(int sz, int id) {
        init(sz, id);
    }

    template <class T>
    DistributedChannelUnbounded<T>::DistributedChannelUnbounded(const DistributedChannelUnbounded<T> &ch){
        localChannel = ch.localChannel;
        channelID = ch.channelID;
        maxsize = ch.maxsize;
        currentProc = ch.currentProc;
        assignedNode = ch.assignedNode;
        m_isClosed = ch.m_isClosed;
        takeIntoAccount = ch.takeIntoAccount;
    }


    template <class T>
    void DistributedChannelUnbounded<T>::deleteRemoteChannel(){

        std::string sentData = DELETE_PREFIX;
        int msgSize = static_cast<int>(sentData.size());

//        std::cout << "DistributedChannelUnbounded<T>::deleteRemoteChannel()  REQUEST CHANNEL MSG " << sentData.data() << " " << " size= " << msgSize << " dest= " << assignedNode << " channelId= " << channelID << std::endl;

        std::this_thread::sleep_for(RndUtils::getRandomMillisecondsTime());
        MPI_Send(sentData.data(), msgSize, MPI_CHAR, assignedNode, channelID, MPI_COMM_WORLD);

//        std::cout << "DistributedChannelUnbounded<T>::deleteRemoteChannel() success " << sentData.data() << " " << " size= " << msgSize << " dest= " << assignedNode << " channelId= " << channelID << std::endl;
    }

    template <class T>
    DistributedChannelUnbounded<T>::~DistributedChannelUnbounded(){
        if (takeIntoAccount){
            channelCount--;
//            if (currentProc == 0){
            if (isRemote()){
               deleteRemoteChannel();
            }
            else {
                DistributedRuntime::deleteChannel(channelID);
            }
//            }
            if (channelCount == 0) {
//                std::cout << "DistributedChannelUnbounded<T>::~DistributedChannelUnbounded: currentproc = " << currentProc << " channelId " << channelID << std::endl;
                if (channelWorker.joinable())
                {
                    channelWorker.join();
                }
                MPI_Comm_free(&writeComm);
                MPI_Comm_free(&readComm);
                MPI_Finalize();
            }
        }
        else{
            delete localChannel;
        }
    }

    template <class T>
    bool DistributedChannelUnbounded<T>::isRemote(){
        return assignedNode != currentProc;
    }

    template <class T>
    void DistributedChannelUnbounded<T>::close(){
        if (takeIntoAccount){
            std::string sentData = CLOSE_PREFIX;
            int msgSize = static_cast<int>(sentData.size());

//            std::cout << "DistributedChannelUnbounded<T>::close()  REQUEST CLOSE CHANNEL assignedNode= "<< assignedNode << " channelId= " << channelID << std::endl;

            MPI_Send(sentData.data(), msgSize, MPI_CHAR, assignedNode, channelID, MPI_COMM_WORLD);
        }else{
            m_isClosed = true;
        }

    }

    template <class T>
    bool DistributedChannelUnbounded<T>::isClosed(){
        return m_isClosed;
    }

    template <class T>
    bool DistributedChannelUnbounded<T>::write(const T& value, const bool wait){
        bool result = false;

        if(isClosed()){
            std::cerr << std::endl << "Error! Process" << currentProc << " tried to write in a closed channel!" << std::endl;
            MPI_Abort(MPI_COMM_WORLD, MPI_ERR_OTHER);
        }

        // if a channel is not in the channels map, then send request to write
        if (takeIntoAccount){
            std::string serialized = hps::to_string(value);
            std::string sentData = WRITE_PREFIX + serialized;
            int msgSize = static_cast<int>(sentData.size());

//            std::cout << currentProc << " DistributedChannelUnbounded<T>::write Request for " << sentData << " " << " size= " << msgSize << " dest= " << assignedNode << " channelId= " << channelID << std::endl;
            std::this_thread::sleep_for(RndUtils::getRandomMillisecondsTime());


            MPI_Send(sentData.data(), msgSize, MPI_CHAR, assignedNode, channelID, MPI_COMM_WORLD);

//            std::cout << currentProc << " DistributedChannelUnbounded<T>::write sent " << sentData.data() << " " << " size= " << msgSize << " dest= " << assignedNode << " channelId= " << channelID << std::endl;

            MPI_Recv(&result, 1, MPI_CXX_BOOL, assignedNode, channelID, writeComm, MPI_STATUS_IGNORE);

//            std::cout << currentProc << " DistributedChannelUnbounded<T>::write received answer size= " << msgSize << " dest= " << assignedNode << " channelId= " << channelID << std::endl;


            if (wait){
                while(!result){
                    std::this_thread::sleep_for(RndUtils::getRandomMillisecondsTime());
                    MPI_Send(sentData.data(), msgSize, MPI_CHAR, assignedNode, channelID, MPI_COMM_WORLD);
                    std::cout << currentProc << " DistributedChannelUnbounded<T>::while write sent" << sentData.data() << " " << " size= " << msgSize << " dest= " << assignedNode << " channelId= " << channelID << std::endl;
                    MPI_Recv(&result, 1, MPI_CXX_BOOL, assignedNode, channelID, writeComm, MPI_STATUS_IGNORE);
                }
//                std::cout << currentProc << " DistributedChannelUnbounded<T>::write received answer after loop size= " << msgSize << " dest= " << assignedNode << " channelId= " << channelID << std::endl;
            }

        }
        else {
//            std::cout << currentProc << " DistributedChannelUnbounded<T>::write local; assignedNode" <<  assignedNode << " channelId= " << channelID << " value= " << value << std::endl;
            result = localChannel->write(value, wait);
        }

//        std::cout << currentProc << " DistributedChannelUnbounded<T>::write return result"<< " dest= " << assignedNode << " channelId= " << channelID << " value= " << value << "wait= " << wait << std::endl;

        return result;
    }

    template <class T>
    bool DistributedChannelUnbounded<T>::read(T* value, bool wait){
        bool result;

        if (takeIntoAccount){
            T val;
            std::string receivedMsg, sentData = READ_PREFIX;
            int msgSize = static_cast<int>(sentData.size());
            int valueSize = 1;
            char *valueRaw;
            MPI_Status status;

            do {
//                std::cout << currentProc << " DistributedChannelUnbounded<T>::read Request for " << sentData << " " << " size= " << msgSize << " dest= " << assignedNode << " channelId= " << channelID << std::endl;
                std::this_thread::sleep_for(RndUtils::getRandomMillisecondsTime());


                MPI_Send(sentData.data(), msgSize, MPI_CHAR, assignedNode, channelID, MPI_COMM_WORLD);

//                std::cout << currentProc << " DistributedChannelUnbounded<T>::read sent " << sentData.data() << " " << " size= " << msgSize << " dest= " << assignedNode << " channelId= " << channelID << std::endl;

                MPI_Probe(assignedNode, channelID, readComm, &status);

                MPI_Get_count(&status, MPI_CHAR, &valueSize);
                valueRaw = new char[valueSize + 1];
                std::fill(valueRaw, valueRaw + valueSize + 1, 0);

                MPI_Recv(valueRaw, valueSize, MPI_CHAR, assignedNode, channelID, readComm, &status);

//                std::cout << currentProc << " DistributedChannelUnbounded<T>::read received answer size= " << msgSize << " dest= " << assignedNode << " channelId= " << channelID << std::endl;

                receivedMsg = valueRaw;

                if (receivedMsg[0] == '0'){
                    result = false;
                }
                else {
                    result = true;
                }

            }while(!result && wait);

//            std::cout << currentProc << " DistributedChannelUnbounded<T>::read return result" << msgSize << " dest= " << assignedNode << " channelId= " << channelID << "rcvm" << receivedMsg<< std::endl;

            // skip the first char because it's used to define the bool result value
            if (valueSize > 1){
                val = hps::from_string<T>(receivedMsg.substr(1));
            }
            *value = val;
            return result;
        }
        else{
//            std::cout << currentProc << " DistributedChannelUnbounded<T>::read local; assignedNode" <<  assignedNode << " channelId= " << channelID << " value= " << *value << std::endl;

            return localChannel->read(value, wait);
        }
    }

/////////////////////////////////////////////
// DistributedRuntime namespace content /////
/////////////////////////////////////////////

    HashRing ring;
    int totalProcs;
    std::map< int, std::pair<DistributedChannelUnbounded<std::string >*, int> >channels;
//    std::map< int, DistributedChannelUnbounded<std::string >*>channels;
    std::mutex m_mutex;


    void init(){
        int provided;
//        MPI_Init(nullptr, nullptr);
        MPI_Init_thread(nullptr, nullptr, MPI_THREAD_MULTIPLE, &provided);

        // Create another communicator when we receive responses for read/write operations
        // because manageChannels() from another process can interact with these responses
        MPI_Comm_dup(MPI_COMM_WORLD, &writeComm);
        MPI_Comm_dup(MPI_COMM_WORLD, &readComm);

        ring = HashRing();
        totalProcs = MPIUtils::getWorldSize();

        // get nodes into the ring
        for(int i = 0; i < totalProcs; i++){
            ring.addNode(i);
        }
        finished = false;
    }

    void manageChannels(){
        while(!finished && !channels.empty()){
//            std::cout << MPIUtils::getCurrentProc()<< " #CHANNELS SIZE#  " << channels.size() << std::endl;
//            std::cout << "#FINISHED #" << finished << std::endl;
            listenToMessages();
        }
    }

    void listenToMessages(){
        MPI_Status status;
        MPI_Request request;
        int msgSize = 1, source, channelId;
        char* maxMsg = new char[MSG_MAX_LENGTH];
        char* recvMsg;


//        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_SOURCE, MPI_COMM_WORLD, &status);
//        MPI_Get_count(&status, MPI_CHAR, &msgSize);

//        msg = new char[msgSize + 1];
//        std::fill(msg, msg + msgSize + 1, 0);

//        source = status.MPI_SOURCE;
//        channelId = status.MPI_TAG;

//        std::cout << MPIUtils::getCurrentProc()<< "listenToMessages():: MPI_Recv "<< std::endl;

//        MPI_Recv(maxMsg, msgSize, MPI_CHAR, source, channelId, MPI_COMM_WORLD, &status);

//        MPI_Recv(maxMsg, MSG_MAX_LENGTH, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);


        MPI_Irecv(maxMsg, MSG_MAX_LENGTH, MPI_CHAR, MPI_ANY_SOURCE,
                  MPI_ANY_TAG, MPI_COMM_WORLD, &request);


        // if we don't receive something in MSG_RECV_TIMEOUT_SEC seconds return
        auto startTime = MPI_Wtime();
        int gotData = 0;
        MPI_Test(&request, &gotData, &status);
        while(!gotData && (MPI_Wtime() - startTime < MSG_RECV_TIMEOUT_SEC)){
            std::this_thread::sleep_for(RndUtils::getRandomMillisecondsTime());
            MPI_Test(&request, &gotData, &status);
        }
        if (!gotData){
            MPI_Cancel(&request);
            delete [] maxMsg;
            return;
        }

        source = status.MPI_SOURCE;
        channelId = status.MPI_TAG;

        MPI_Get_count(&status, MPI_CHAR, &msgSize);

        recvMsg = new char[msgSize + 1];
        std::fill(recvMsg, recvMsg + msgSize + 1, 0);
        strncpy(recvMsg, maxMsg, msgSize);

//        std::cout << MPIUtils::getCurrentProc()<< "listenToMessages():: MESSAGE RECEIVED = " << recvMsg << " SOURCE = " << status.MPI_SOURCE <<  "TAG = " << status.MPI_TAG << " SIZE= " << msgSize << std::endl;

        std::string receivedMsg(recvMsg);

        if (receivedMsg.rfind(WRITE_PREFIX, 0) == 0) {
            std::string content = receivedMsg.substr(WRITE_PREFIX.size());
            bool result;

            // never wait here if we try to write to a channel that is in the channels map <string>
            // we want to have the blocking behavoir where the channel.write was called locally
            result = channels[channelId].first->write(content, false);
            MPI_Send(&result, 1, MPI_CXX_BOOL, source, channelId, writeComm);
//            std::cout << MPIUtils::getCurrentProc() << "listenToMessages():: SEND write answer " << result << " to "
//                      << status.MPI_SOURCE << " TAG = " << status.MPI_TAG << " SIZE= " << 1 << std::endl;

        } else if (receivedMsg.rfind(READ_PREFIX, 0) == 0) {
            std::string content;
            std::string toSend;
            bool result;

            result = channels[channelId].first->read(&content, false);
            if(result){
                toSend = "1" + content;
            }
            else {
                toSend = "0";
            }
            int toSendSize = static_cast<int>(toSend.size());
            MPI_Send(toSend.data(), toSendSize, MPI_CHAR, source, channelId, readComm);

        } else if (receivedMsg.rfind(CLOSE_PREFIX, 0) == 0) {
            if (channels.find(channelId) != channels.end()) {
                channels[channelId].first->close();
            }

        } else if (receivedMsg.rfind(DELETE_PREFIX, 0) == 0){
//            std::cout << MPIUtils::getCurrentProc() << "DELETE IF " << status.MPI_SOURCE << " TAG = " << status.MPI_TAG << " SIZE= " << 1 << std::endl;
            deleteChannel(channelId);
        }
        delete [] maxMsg;
        delete [] recvMsg;
    }

    void run(){
//        if (MPIUtils::getCurrentProc() == 0)//{
            channelWorker = std::thread(&manageChannels);
//        } else{
//            manageChannels();
//
//            // End program
//            MPI_Comm_free(&writeComm);
//            MPI_Comm_free(&readComm);
//            MPI_Finalize();
//            std::exit(0);
//        }

    }
    int assignChannelToNode(int id){
        return ring.assignKeyToNode(std::to_string(id % (ring.getSize() + 1)));

    }
    void addChannel(int id, int size){
        std::unique_lock<std::mutex> lock(m_mutex);
        if (size > 0) {
            // create remote channel
            channels[id] = std::make_pair(new DistributedChannelUnbounded<std::string>(size, id), 0);
        } else { // Create bounded channel

        }

//        std::cout << MPIUtils::getCurrentProc()<< "Added channel with id=" << id << " and size=" << size << std::endl;

    }
    void deleteChannel(int id){
        std::unique_lock<std::mutex> lock(m_mutex);
        channels[id].second++;

        if (channels[id].second == MPIUtils::getWorldSize()){
//            std::cout << MPIUtils::getCurrentProc()<< " DistributedRuntime::deleteChannel() deleting: " << id << std::endl;
            delete channels[id].first;
            channels.erase(id);
        }
        else {
//            std::cout << MPIUtils::getCurrentProc()<< " DistributedRuntime::deleteChannel() request for delete " << id << "have only" <<  channels[id].second << std::endl;
        }
        if (channels.empty()){
//            std::cout << MPIUtils::getCurrentProc()<< " YES\n";
            finished = true;
        }

    }

}

#endif //DISTRIBUTED_CSP_DISTRIBUTEDRUNTIME_H
