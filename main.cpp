#include<iostream>
#include<vector>
#include<algorithm>
#include<sstream>
#include<fstream>
#include<cmath>
#include<string>

// memory errors (invalid read and writes) 10%
// memory leaks 5%
// makefile + clean

using namespace std;

#define ull unsigned long long

struct CacheBlock {
    bool validBit = false;
    // bool dirtyBit = false;
    ull tagBits = 0;
    // unsigned long long lastUsed = 0; 
};

class Cache {

    private:
        ull sets = 0 ;
        ull blocks = 0;
        ull bytes = 0 ;
        bool writeAllocate = false;
        bool writeThrough = false;
        bool fifo = false;

        ull loads=0;
        ull stores=0;
        ull loadHits=0;
        ull loadMisses=0;
        ull storeHits=0;
        ull storeMisses=0;
        ull cycles=0;

        vector<vector<CacheBlock>> cache;

    public:

        Cache(ull setsp,ull blocksp,ull bytesp,bool writeAllocatep,bool writeThroughp, bool fifop){
            sets = setsp;
            blocks = blocksp;
            bytes = bytesp;
            writeAllocate = writeAllocatep;
            writeThrough = writeThroughp;
            fifo = fifop;

            cache.resize(sets, vector<CacheBlock>(blocks));
        }


        void load(ull adr, ull setIndexBits,ull blockOffsetBits,ull tagBits,ull setNum,ull offset,bool hit){
            loads++;
            cout << "load " << adr << endl;
            
            if(hit){
                loadHits++;
            }else{
                loadMisses++;
            }
        };


        void store(ull adr, ull setIndexBits,ull blockOffsetBits,ull tagBits,ull setNum,ull offset,bool hit){
            stores++;
            cout << "store " << adr << endl;
            cout << setIndexBits << " "  << blockOffsetBits << " " << tagBits << " " << setNum << " " << offset << endl;
            
            if(hit){
                storeHits++;
            }else{
                storeMisses++;
            }
        };


        void updateCache(char type, string& address){

            // calculate address
            stringstream ss;
            ss << hex << address;
            ull adr;
            ss >> adr;

            // use address and cache parameters to calculate data relevant to that address
            int setIndexBits = log2(sets);
            int blockOffsetBits = log2(bytes);
            // int tagBits = 32 - setIndexBits - blockOffsetBits;

            ull setNum = (adr/bytes) % sets;
            ull offset = adr % bytes;
            ull tagValue = adr >> (setIndexBits + blockOffsetBits);

            // check if it is a hit
            bool hit = false;
            for(auto& block : cache[setIndexBits]){
                if(block.validBit && block.tagBits == tagValue){
                    hit = true;
                }
            }

            if(type == 'l'){
                cycles++;
                load(adr,setIndexBits,blockOffsetBits,tagValue,setNum,offset,hit);
            }
            else if(type == 's'){
                cycles++;
                store(adr,setIndexBits,blockOffsetBits,tagValue,setNum,offset,hit);
            }
        }
};



int main(){

    Cache cache(4,4,4,true,true,true);

    ifstream fin("trace_file_test");
    string line;
    string a, b, c;

    while (getline(fin, line))
    {
        istringstream ss(line);
        ss >> a >> b >> c ;

        char type = a[0];
        // cout << a << " " << b << " " << c << endl;
        cache.updateCache(type,b);
        
    }

    return 0;
}