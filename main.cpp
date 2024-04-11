#include<iostream>
#include<vector>
#include<algorithm>

// memory errors (invalid read and writes) 10%
// memory leaks 5%
// makefile + clean

using namespace std;

#define ull unsigned long long

struct CacheBlock {
    bool validBit = false;
    bool dirtyBit = false;
    ull tagBits = 0;
    // unsigned long long lastUsed = 0; 
};

class Cache {
    private:
        ull sets;
        ull blocks;
        ull bytes;
        bool writeAllocate;
        bool writeThrough;
        bool fifo;

        vector<vector<CacheBlock>> cache;

    public:
        Cache(ull sets,ull blocks,ull bytes,bool writeAllocate,bool writeThrough, bool fifo){
            sets = sets;
            blocks = blocks;
            bytes = bytes;
            writeAllocate = writeAllocate;
            writeThrough = writeThrough;
            fifo = fifo;

            cache.resize(sets, vector<Cache>(blocks));
        }
};



int main(){

    

    return 0;
}