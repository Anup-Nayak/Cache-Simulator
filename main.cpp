#include<iostream>
#include<vector>
#include<algorithm>
#include<sstream>
#include<fstream>
#include<cmath>
#include<string>
#include<queue>
#include<climits>

using namespace std;

#define ull unsigned long long


struct Node {
    int data;
    Node *next, *prev;

    Node(int val){
        data = val;
        next = nullptr;
        prev = nullptr;
    }
};

class LinkedList {

public:
    Node *head;
    Node *tail;
    LinkedList(){
        head = NULL;
        tail = NULL;
        
    }

    void insertAtLast(int val) {
        Node *newNode = new Node(val);
        if (!head) {
            head = newNode;
            tail = newNode;
        } else {
            newNode->prev = tail;
            tail->next = newNode;
            tail = newNode;
        }
    }
    
    int removeFromTop() {
        int data = -1;
        if (head != NULL) {
            Node *temp = head;
            data = temp->data;
            head = head->next;
            temp->next = NULL;
            delete temp;
            temp = NULL;
            if (head) {
                head->prev = NULL;
            } else {
                tail = NULL;
            }
        }
        return data;
    }

    void transferToEnd(Node *n) {
        if (n->next == NULL) return;
        if (n->prev == NULL) {
            head = n->next;
            head->prev = NULL;
            tail->next = n;
            n->prev = tail;
            n->next = NULL;
            tail = n;
            return;
        }
        Node *p = n->prev;
        Node *ne = n->next;
        p->next = ne;
        ne->prev = p;
        tail->next = n;
        n->prev = tail;
        tail = n;
        n->next = NULL;
    }

    ~LinkedList() {
        while (head) {
            removeFromTop();
        }
    }
};


struct CacheBlock {
    bool validBit = false;
    bool dirtyBit = false;
    ull tagBits = 0;
    ull setIndex = -1;
    
};

struct Set {
    vector<CacheBlock> SetBlocks;
    queue <int> fifo;
    LinkedList lru;
    unordered_map<int,Node* > data;
    Set(){
        lru = LinkedList();
    }
    // vector<int> ageBits;
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


        void load(ull tagValue,ull setNum,bool hit){
            
            loads++;
            
            if(hit){
                loadHits++;
                cycles++;
                
                if(!fifo){
                    Node *curr = cache[setNum].data[tagValue];
                    cache[setNum].lru.transferToEnd(curr);
                    cache[setNum].data[tagValue] = cache[setNum].lru.tail;
                }
                
            }else{
                
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
                    bool emptyBlock = false;
                    for(auto& block: cache[setNum].SetBlocks){
                        if(block.validBit == false){
                            allocatedIndex = block.setIndex;
                            emptyBlock=true;
                            cache[setNum].lru.insertAtLast(allocatedIndex);
                            cache[setNum].data[tagValue] = cache[setNum].lru.tail;
                            break;
                        }
                    }

                    if(!emptyBlock){
                        allocatedIndex = cache[setNum].lru.removeFromTop();
                        cache[setNum].lru.insertAtLast(allocatedIndex);
                        cache[setNum].data[tagValue] = cache[setNum].lru.tail;
                    }

                }
                if(writeThrough){
                    cycles = cycles + 1 + 100*(bytes/4);
                }else{
                    if(cache[setNum].SetBlocks[allocatedIndex].dirtyBit){
                        cycles = cycles + 1 + 100*(bytes/4) + 100*(bytes/4);
                    }else{
                        cycles = cycles + 1 + 100*(bytes/4);
                    }
                }
                if(cache[setNum].SetBlocks[allocatedIndex].dirtyBit == true){
                    
                    writeToMemory++;
                    cache[setNum].SetBlocks[allocatedIndex].validBit = true;
                    cache[setNum].SetBlocks[allocatedIndex].tagBits = tagValue;
                    cache[setNum].SetBlocks[allocatedIndex].dirtyBit = false;
                }else{
                    
                    cache[setNum].SetBlocks[allocatedIndex].validBit = true;
                    cache[setNum].SetBlocks[allocatedIndex].tagBits = tagValue;
                }
                
                loadMisses++;
                readFromMemory++;
            }
        };


        void store(ull tagValue,ull setNum,bool hit,ull hitIndex){
            stores++;            

            if(hit){
                
                storeHits++;

                if(writeThrough){
                    cycles = cycles + 1 + 100;
                    writeToMemory++;

                }else{
                    cycles++;
                    cache[setNum].SetBlocks[hitIndex].dirtyBit = true;                    
                }
                if(!fifo){
                    
                    Node *curr = cache[setNum].data[tagValue];
                    cache[setNum].lru.transferToEnd(curr);
                    cache[setNum].data[tagValue] = cache[setNum].lru.tail;
                    
                }
            }else{
                
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

                        bool emptyBlock = false;
                        for(auto& block: cache[setNum].SetBlocks){
                            if(block.validBit == false){
                                allocatedIndex = block.setIndex;
                                emptyBlock=true;
                                cache[setNum].lru.insertAtLast(allocatedIndex);
                                cache[setNum].data[tagValue] = cache[setNum].lru.tail;
                                break;
                            }
                        }

                        if(!emptyBlock){
                            allocatedIndex = cache[setNum].lru.removeFromTop();
                            cache[setNum].lru.insertAtLast(allocatedIndex);
                            cache[setNum].data[tagValue] = cache[setNum].lru.tail;
                        }

                
                    }

                    if(writeThrough){
                        cycles = cycles + 101 + 100*(bytes/4);
                    }else{
                        if(cache[setNum].SetBlocks[allocatedIndex].dirtyBit){
                            cycles = cycles + 200*(bytes/4) + 1;
                        }else{
                            cycles = cycles + 100*(bytes/4) + 1;
                        }
                    }
                    
                    if(cache[setNum].SetBlocks[allocatedIndex].dirtyBit == true){
                        writeToMemory++;
                        cache[setNum].SetBlocks[allocatedIndex].dirtyBit = false;
                        
                    }
                    if(writeThrough){
                        
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
                    cycles = cycles + 101;
                }
            }
            

            
        };


        void updateCache(char type, string& address){

            
            stringstream ss;
            ss << hex << address;
            ull adr;
            ss >> adr;

            
            int setIndexBits = log2(sets);
            int blockOffsetBits = log2(bytes);

            ull setNum = (adr/bytes) % sets;
            ull tagValue = adr >> (setIndexBits + blockOffsetBits);

            
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
                
                load(tagValue,setNum,hit);
            }
            else if(type == 's'){
                
                store(tagValue,setNum,hit,index);
            }
        }
        

        void cleanup(){
            for(auto& set1: cache){
                for(auto& block1: set1.SetBlocks){
                    if(block1.dirtyBit == true){
                        cycles += 100*(bytes/4);
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
            cout << "Total cycles: " << cycles << endl;
        }
};

int main(int , char* argv[]) {

    ull sets = stoi(argv[1]);
    ull blocks = stoi(argv[2]);
    ull bytes = stoi(argv[3]);
    string wa = argv[4];
    string wt = argv[5];
    string f = argv[6];

    bool writeAllocate = (wa == "write-allocate");
    bool writeThrough = (wt == "write-through");
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