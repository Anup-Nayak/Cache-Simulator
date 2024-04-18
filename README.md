**CACHE SETUP:**

Cache is setup as a vector of *Sets*, each set is a vector of *CacheBlock*.

CacheBlock stores the attributes of *validBit, dirtyBit, tagBits and its Index in the Set*.

Sets along with the CacheBlock vector, store data relevant for implementation of *eviction strategies.*



**CACHE PARAMETERS:**

• number of sets in the cache (a positive power-of-2)
• number of blocks in each set (a positive power-of-2)
• number of bytes in each block (a positive power-of-2, at least 4)
• write-allocate or no-write-allocate
• write-through or write-back
• lru (least-recently-used) or fifo evictions



**DESIGN DECISIONS:**

1. *Compulsory Load Misses* require readFromMemory.
2. Load Misses are allocated memory in Cache based on *eviction strategy.*
3. Load Misses requiring to write over a CacheBlock that has *dirtyBit* set as true requires writeToMemory.
4. Store Hits in case of *writeThrough* requires a writeToMemory.
5. Store Hits in case of *writeBacck* requires only setting *dirtyBit*.
6. Store Misses with *no-write-allocate* does a writeToMemory.
7. Store Misses with write-allocate works with both write-through and write-back.
8. *fifo* uses a simple *queue* storing index information for each Set.
9. *LRU* is implemented using *linked-list and unordered_map* that are maintained for each CacheBlock in all Sets.
10. fifo and LRU implementation in store and load remains similar.
11. after all lines of the program are over, cache is checked for any dirty-bits remaining which are pushed into the memory when found. This ensures no data-hazards occur due to mismatch of data in cache and main memory.

**OPTIMIZATIONS**

1. In case of a load/store hit with that particular CacheBlock having dirtyBit, data is not written in Memory at that time to save cycles.

2. *LRU* uses linked-list approach of a queue like data-structure.



**EXECUTION:**

To compile and create executable:

`make`

To run the executable with cache parameters (change the parameters and input file as required):

`./cacheSim 256 64 16 write-allocate write-through fifo < testgen.txt`

To clean the object and executable files:

`make clean`


**TESTING**

This implementation of CacheSimulator has been rigorously tested with simple hand-written testcases as well as automated Big-Test-Cases.

All trace-files have also been run.