#include "../include/HashRing.h"
#include <iostream>

HashRing::HashRing() {}


unsigned long HashRing::getSize(){
    return nodes.size();
}

uint32_t HashRing::fnv1(const char *data) {
    uint32_t hash = default_offset_basis;
    for (const char *byte = data; *byte != '\0'; ++byte) {
        hash *= prime;
        hash ^= *byte;
    }
    return hash;
}

void HashRing::addNode(int node) {
       std::string nodeName = std::to_string(node);
       uint32_t hash = fnv1(nodeName.data());
       nodes[hash] = node;
       //std::cout <<"Adding node" << " " << nodeName << " "<< hash << " " << node << std::endl;

}


void HashRing::deleteNode(int node) {
    std::string nodeName = std::to_string(node);
    uint32_t hash = fnv1(nodeName.data());
    nodes.erase(hash);
}

int HashRing::assignKeyToNode(std::string key){
    uint32_t hash = fnv1(key.data());
    int nodeNumber = nodes.upper_bound(hash)->second;
//    std::cout << "hash=" << hash << " key=" << key << " " << "nodeNumber= " << nodeNumber << " first "<< this->nodes.upper_bound(hash)->first << std::endl;
    return nodeNumber;
}