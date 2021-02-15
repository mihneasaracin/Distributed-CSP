#ifndef DISTRIBUTED_CSP_HASHRING_H
#define DISTRIBUTED_CSP_HASHRING_H

#include <map>
#include <string>
#include <vector>

class HashRing {

  private:
    static constexpr uint32_t default_offset_basis = 0x811C9DC5;
    static constexpr uint32_t prime = 0x01000193;
    std::map<uint32_t, int> nodes;


  public:
    HashRing();
    ~HashRing()= default;
    uint32_t fnv1(const char* data);
    void addNode(int node);
    void deleteNode(int node);
    int assignKeyToNode(std::string key);
    unsigned long getSize();


};


#endif //DISTRIBUTED_CSP_HASHRING_H
