#include<iostream>
#include<vector>
#include<algorithm>
#include<sstream>
#include<fstream>

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

        ull loads;
        ull stores;
        ull loadHits;
        ull loadMisses;
        ull storeHits;
        ull storeMisses;
        ull cycles;

        vector<vector<CacheBlock>> cache;

    public:

        Cache(ull sets,ull blocks,ull bytes,bool writeAllocate,bool writeThrough, bool fifo){
            sets = sets;
            blocks = blocks;
            bytes = bytes;
            writeAllocate = writeAllocate;
            writeThrough = writeThrough;
            fifo = fifo;


            loads = 0;
            stores = 0;
            loadHits = 0;
            loadMisses = 0;
            storeHits = 0;
            storeMisses = 0;
            cycles = 0;


            cache.resize(sets, vector<CacheBlock>(blocks));
        }

        void load(ull adr, ull index, ull tag){
            cout << adr << endl;
        };


        void store(ull adr, ull index, ull tag){
            cout << "store" << endl;
        };


        void updateCache(char type, string& address){

            stringstream ss;
            ss << hex << address;
            ull adr;
            ss >> adr;

            // ull index = (adr/blocks) % sets;
            // ull tag = adr / (blocks*sets);

            // ull adr = 0;
            ull index = 0;
            ull tag = 0;

            if(type == 'l'){
                cycles++;
                load(adr,index,tag);
            }
            else if(type == 's'){
                cycles++;
                store(adr,index,tag);
            }
        }
};



int main(){

    Cache cache(0,0,0,true,true,true);

    ifstream fin("trace_file");
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