#include<iostream>
#include<vector>
#include<algorithm>
#include<sstream>
#include<fstream>
#include<cmath>
#include<string>
#include<queue>

// memory errors (invalid read and writes) 10%
// memory leaks 5%
// makefile + clean

using namespace std;

#define ull unsigned long long

struct CacheBlock {
    bool validBit = false;
    bool dirtyBit = false;
    ull tagBits = 0;
    ull setIndex = -1;
    
};

struct Set {
    vector<CacheBlock> SetBlocks;
    queue <int> fifo;
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
        ull writeToMemory=0;
        ull readFromMemory=0;

        vector<Set> cache;

    public:

        Cache(ull setsp,ull blocksp,ull bytesp,bool writeAllocatep,bool writeThroughp, bool fifop){
            sets = setsp;
            blocks = blocksp;
            bytes = bytesp;
            writeAllocate = writeAllocatep;
            writeThrough = writeThroughp;
            fifo = fifop;

            cache.resize(sets);
            for(ull i =0; i<sets; i++){
                cache[i].SetBlocks.resize(blocks, CacheBlock());
                for(ull j=0;j<blocks;j++){
                    cache[i].SetBlocks[j].setIndex = j;
                }
            }
            
        }


        void load(ull adr, ull setIndexBits,ull blockOffsetBits,ull tagValue,ull setNum,ull offset,bool hit,ull index){
            
            loads++;
            
            if(hit){
                loadHits++;
                cout << "load hit" << endl;
            }else{
                
                cout << "load miss" << endl;
                ull index = 0;
                if(cache[setIndexBits].fifo.size() != blocks){
                    for(auto &block : cache[setIndexBits].SetBlocks){
                        if (block.validBit == false){
                            index = block.setIndex;
                            cache[setIndexBits].fifo.push(index);
                            break;
                        }
                    }
                }else{
                    index = cache[setIndexBits].fifo.front();
                    cache[setIndexBits].fifo.pop();
                    cache[setIndexBits].fifo.push(index);
                }
                if(cache[setIndexBits].SetBlocks[index].dirtyBit == true){
                    // store to main memory
                    writeToMemory++;
                    cache[setIndexBits].SetBlocks[index].validBit = true;
                    cache[setIndexBits].SetBlocks[index].tagBits = tagValue;
                    cache[setIndexBits].SetBlocks[index].dirtyBit = false;
                }else{
                    cache[setIndexBits].SetBlocks[index].validBit = true;
                    cache[setIndexBits].SetBlocks[index].tagBits = tagValue;
                }
                loadMisses++;
                readFromMemory++;
            }
        };


        void store(ull adr, ull setIndexBits,ull blockOffsetBits,ull tagValue,ull setNum,ull offset,bool hit,ull index){
            stores++;            

            if(hit){
                cout << "store hit" << endl;
                storeHits++;
                if(writeThrough){
                    //writeThrough
                    writeToMemory++;

                }else{
                    //writeBack
                    cache[setIndexBits].SetBlocks[index].dirtyBit == true;                    
                }
            }else{
                cout << "store miss" << endl;
                storeMisses++;
                if(writeAllocate){
                    ull allocatedIndex = 0;       
                    if(cache[setIndexBits].fifo.size() != blocks){
                        for(auto &block : cache[setIndexBits].SetBlocks){
                            if (block.validBit == false){
                                allocatedIndex = block.setIndex;
                                cache[setIndexBits].fifo.push(allocatedIndex);
                                break;
                            }
                        }
                    }else{
                        allocatedIndex = cache[setIndexBits].fifo.front();
                        cache[setIndexBits].fifo.pop();
                        cache[setIndexBits].fifo.push(allocatedIndex);
                    }
                    if(cache[setIndexBits].SetBlocks[allocatedIndex].dirtyBit == true){
                        writeToMemory++;
                        cache[setIndexBits].SetBlocks[allocatedIndex].dirtyBit = false;
                    }
                    if(writeThrough){
                        // find out allocated index
                        cache[setIndexBits].SetBlocks[allocatedIndex].validBit = true;
                        cache[setIndexBits].SetBlocks[allocatedIndex].tagBits = tagValue;
                        writeToMemory++;
                    }else{
                        cache[setIndexBits].SetBlocks[allocatedIndex].dirtyBit == true;                    
                        cache[setIndexBits].SetBlocks[allocatedIndex].validBit = true;
                        cache[setIndexBits].SetBlocks[allocatedIndex].tagBits = tagValue;
                    }
                }else{
                    writeToMemory++;
                }
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

            ull setNum = (adr/bytes) % sets;
            ull offset = adr % bytes;
            ull tagValue = adr >> (setIndexBits + blockOffsetBits);

            // check if it is a hit
            bool hit = false;
            ull index = 0;
            for(auto& block : cache[setIndexBits].SetBlocks){
                index++;
                if(block.validBit && block.tagBits == tagValue){
                    hit = true;
                    break;
                }
            }

        
            if(type == 'l'){
                cycles++;
                load(adr,setIndexBits,blockOffsetBits,tagValue,setNum,offset,hit,index);
            }
            else if(type == 's'){
                cycles++;
                store(adr,setIndexBits,blockOffsetBits,tagValue,setNum,offset,hit,index);
            }
        }
        

        void cleanup(){
            for(auto& set1: cache){
                for(auto& block1: set1.SetBlocks){
                    if(block1.dirtyBit == true){
                        writeToMemory++;
                    }
                }
            }
        }
        void print_statistics(){
            cout << "Total loads: " << loads << endl;
            cout << "Total stores: " << stores << endl;
            cout << "Load hits: " << loadHits << endl;
            cout << "Load misses: " << loadMisses << endl;
            cout << "Store hits: " << storeHits << endl;
            cout << "Store misses: " << storeMisses << endl;
            cout << "Total cycles: " << cycles + 100*(writeToMemory+readFromMemory)*((bytes)/4) << endl;
        }
};

int main(int argc, char* argv[]) {

    ull sets = stoi(argv[1]);
    ull blocks = stoi(argv[2]);
    ull bytes = stoi(argv[3]);
    string wa = argv[4];
    string wb = argv[5];
    string f = argv[6];

    bool writeAllocate = (wa == "write-allocate");
    bool writeThrough = (wb == "write-through");
    bool fifo = (f == "fifo");

    Cache cache(sets,blocks,bytes,writeAllocate,writeThrough,fifo);

    string a;
    string b;
    string c;

    while(cin >> a){
        char type= a[0];
        cin >> b;
        cin >> c;
        cache.updateCache(type,b);

    }
    cache.cleanup();
    cache.print_statistics();

    return 0;
}

// int main(){
//     Cache cache(4,4,16,true,false,true);

//     ifstream fin("testgen.txt");
//     string line;
//     string a,b,c;

//     while(getline(fin,line)){
//         istringstream ss(line);
//         ss >> a >> b >> c;

//         char type = a[0];

//         cache.updateCache(type,a);
//     }
//     cache.cleanup();
//     cache.print_statistics();


// }