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
    vector<int> ageBits;
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
                cache[i].ageBits.resize(blocks,0);
                for(ull j=0;j<blocks;j++){
                    cache[i].SetBlocks[j].setIndex = j;
                    cache[i].ageBits[j] = j;
                }
                
            }
            
        }


        void load(ull tagValue,ull setNum,bool hit,ull index){
            
            loads++;
            
            if(hit){
                loadHits++;
                if(!fifo){
                    cache[setNum].ageBits[index] = blocks;
                    for(ull i=0;i<blocks;i++){
                        cache[setNum].ageBits[i]--;
                    }
                }
                // cout << "load hit" << endl;
            }else{
                
                // cout << "load miss" << endl;
                ull index = 0;
                if(fifo){
                    if(cache[setNum].fifo.size() != blocks){
                        for(auto &block : cache[setNum].SetBlocks){
                            if (block.validBit == false){
                                index = block.setIndex;
                                cache[setNum].fifo.push(index);
                                break;
                            }
                        }
                    }else{
                        index = cache[setNum].fifo.front();
                        cache[setNum].fifo.pop();
                        cache[setNum].fifo.push(index);
                    }
                }else{
                    vector<int> vec = cache[setNum].ageBits;
                    auto minIter = min_element(vec.begin(), vec.end());
                    index = minIter - vec.begin();

                    cache[setNum].ageBits[index] = blocks;
                    for(ull i=0;i<blocks;i++){
                        cache[setNum].ageBits[i]--;
                    }
                }
                if(cache[setNum].SetBlocks[index].dirtyBit == true){
                    // store to main memory
                    writeToMemory++;
                    cache[setNum].SetBlocks[index].validBit = true;
                    cache[setNum].SetBlocks[index].tagBits = tagValue;
                    cache[setNum].SetBlocks[index].dirtyBit = false;
                }else{
                    cache[setNum].SetBlocks[index].validBit = true;
                    cache[setNum].SetBlocks[index].tagBits = tagValue;
                }
                loadMisses++;
                readFromMemory++;
            }
        };


        void store(ull tagValue,ull setNum,bool hit,ull index){
            stores++;            

            if(hit){
                // cout << "store hit" << endl;
                storeHits++;
                if(writeThrough){
                    //writeThrough
                    writeToMemory++;

                }else{
                    //writeBack
                    cache[setNum].SetBlocks[index].dirtyBit = true;                    
                }
                if(!fifo){
                    cache[setNum].ageBits[index] = blocks;
                    for(ull i=0;i<blocks;i++){
                        cache[setNum].ageBits[i]--;
                    }
                }
            }else{
                // cout << "store miss" << endl;
                storeMisses++;
                if(writeAllocate){
                    ull allocatedIndex = 0;       
                    if(fifo){
                        if(cache[setNum].fifo.size() != blocks){
                            for(auto &block : cache[setNum].SetBlocks){
                                if (block.validBit == false){
                                    allocatedIndex = block.setIndex;
                                    cache[setNum].fifo.push(allocatedIndex);
                                    break;
                                }
                            }
                        }else{
                            allocatedIndex = cache[setNum].fifo.front();
                            cache[setNum].fifo.pop();
                            cache[setNum].fifo.push(allocatedIndex);
                        }
                    }else{
                        vector<int> vec = cache[setNum].ageBits;
                        auto minIter = min_element(vec.begin(), vec.end());
                        allocatedIndex = minIter - vec.begin();

                        cache[setNum].ageBits[allocatedIndex] = blocks;
                        for(ull i=0;i<blocks;i++){
                            cache[setNum].ageBits[i]--;
                        }
                    }
                    if(cache[setNum].SetBlocks[allocatedIndex].dirtyBit == true){
                        writeToMemory++;
                        cache[setNum].SetBlocks[allocatedIndex].dirtyBit = false;
                    }
                    if(writeThrough){
                        // find out allocated index
                        cache[setNum].SetBlocks[allocatedIndex].validBit = true;
                        cache[setNum].SetBlocks[allocatedIndex].tagBits = tagValue;
                        writeToMemory++;
                    }else{
                        cache[setNum].SetBlocks[allocatedIndex].dirtyBit = true;                    
                        cache[setNum].SetBlocks[allocatedIndex].validBit = true;
                        cache[setNum].SetBlocks[allocatedIndex].tagBits = tagValue;
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
            ull tagValue = adr >> (setIndexBits + blockOffsetBits);

            // check if it is a hit
            bool hit = false;
            ull index = 0;
            for(auto& block : cache[setNum].SetBlocks){
                index++;
                if(block.validBit && block.tagBits == tagValue){
                    hit = true;
                    break;
                }
            }

        
            if(type == 'l'){
                cycles++;
                load(tagValue,setNum,hit,index);
            }
            else if(type == 's'){
                cycles++;
                store(tagValue,setNum,hit,index);
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