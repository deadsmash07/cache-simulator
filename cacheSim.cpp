#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <fstream>

struct CacheLine
{
    bool valid = false;
    bool dirty = false;
    unsigned long tag = 0;
    int lastUsed = 0; // for LRU policy
};

class CacheSimulator
{
public:
    std::vector<std::queue<int>> fifoQueues; // FIFO queues for each set
    std::vector<std::vector<CacheLine>> cache;
    int sets, blocksPerSet, blockSize;
    bool writeAllocate, writeBack, useLRU;
    int loadHits, loadMisses, storeHits, storeMisses, totalCycles;
    const int memoryAccessCycles = 100; // Assumed penalty for memory access on misses
    // modifying the penalty for clocj

    CacheSimulator(int numSets, int numBlocks, int blockSize, bool writeAlloc, bool writeBack, bool lru)
        : sets(numSets), blocksPerSet(numBlocks), blockSize(blockSize),
          writeAllocate(writeAlloc), writeBack(writeBack), useLRU(lru)
    {
        cache.resize(sets, std::vector<CacheLine>(blocksPerSet));
        fifoQueues.resize(sets);
        for (int i = 0; i < sets; ++i)
        {
            for (int j = 0; j < blocksPerSet; ++j)
            {
                fifoQueues[i].push(j);
            }
        }
        loadHits = loadMisses = storeHits = storeMisses = totalCycles = 0;
    }

    void processAccess(char type, unsigned long address)
    {
        unsigned long blockAddress = address / blockSize;
        unsigned long setIndex = blockAddress % sets;
        unsigned long tag = blockAddress / sets;

        bool hit = false;
        int freeIndex = -1;
        int lruIndex = 0;
        int replaceIndex = -1;

        for (int i = 0; i < blocksPerSet; ++i)
        {
            if (cache[setIndex][i].valid && cache[setIndex][i].tag == tag)
            {
                hit = true;
                if (useLRU)
                    cache[setIndex][i].lastUsed = totalCycles; // Update usage time for LRU
                break;
            }
            if (!cache[setIndex][i].valid && freeIndex == -1)
            {
                freeIndex = i;
            }
            if (useLRU && cache[setIndex][lruIndex].lastUsed > cache[setIndex][i].lastUsed)
            {
                lruIndex = i;
            }
        }

        if (!hit)
        {
            if (freeIndex != -1)
            {
                replaceIndex = freeIndex;
            }
            else if (useLRU)
            {
                replaceIndex = lruIndex;
            }
            else
            {
                replaceIndex = fifoQueues[setIndex].front();
                fifoQueues[setIndex].pop();
                fifoQueues[setIndex].push(replaceIndex);
            }

            if (cache[setIndex][replaceIndex].valid && writeBack && cache[setIndex][replaceIndex].dirty)
            {
                totalCycles += 100*(blockSize/4); // Write back dirty block
            }
            cache[setIndex][replaceIndex].valid = true;
            cache[setIndex][replaceIndex].tag = tag;
            cache[setIndex][replaceIndex].dirty = (type == 's');
            if (useLRU)
                cache[setIndex][replaceIndex].lastUsed = totalCycles; // Update usage time for LRU
            totalCycles += 100*(blockSize/4);                        // Memory access for loading the block
        }
        if( type== 's' && hit && ! writeBack){
            totalCycles += 100; // store hit takes 100 cycles
        }

        updateStatistics(type, hit);
    }

    void updateStatistics(char type, bool hit, unsigned long setIndex = -1, int replaceIndex = -1)
    {
        if (type == 'l')
        {
            if (hit){
                loadHits++;
                totalCycles += 1;} // load hit takes 1 cycle
            else{
                loadMisses++;
                }// load miss takes 100 cycles
        }
        else if (type == 's')
        {
            if (hit){
                storeHits++;
                if (writeBack){
                    totalCycles += 1;} // store hit takes 1 cycle
                // else{
                //     totalCycles += 100; // store hit takes 100 cycles
                // }
              }

            else{ // store miss
                storeMisses++;
                // if(writeBack){
                //     if(writeAllocate){
                //         totalCycles += 100*blockSizeInWords; // store miss takes 100 cycles
                //         if(cache[setIndex][replaceIndex].valid && cache[setIndex][replaceIndex].dirty){
                //             totalCycles += 100*blockSizeInWords; // write back dirty block
                //         }
                //     }
                // }
                // else{
                //     if(writeAllocate){
                //         totalCycles += 100*blockSizeInWords; // store miss takes 100 cycles
                //     }
                //     totalCycles += 100; // store miss takes 100 cycles
                // }
                if( ! writeBack && !writeAllocate){
                    totalCycles += 100; // store miss takes 100 cycles
                }
                
               
            }
        }
        totalCycles += 1; // 1 cycle for processing the access
    }

    void printSummary()
    {
        std::cout << "Total loads: " << loadHits + loadMisses << std::endl;
        std::cout << "Total stores: " << storeHits + storeMisses << std::endl;
        std::cout << "Load hits: " << loadHits << std::endl;
        std::cout << "Load misses: " << loadMisses << std::endl;
        std::cout << "Store hits: " << storeHits << std::endl;
        std::cout << "Store misses: " << storeMisses << std::endl;
        std::cout << "Total cycles: " << totalCycles << std::endl;

        // Now write the same summary to a file
        std::ofstream outf("out.txt"); // Note that 'fstream' is not required here, 'ofstream' is sufficient.

        outf << loadHits + loadMisses << std::endl;
        outf  << storeHits + storeMisses << std::endl;
        outf  << loadHits << std::endl;
        outf << loadMisses << std::endl;
        outf << storeHits << std::endl;
        outf  << storeMisses << std::endl;
        outf  << totalCycles << std::endl;
        outf.close();
    }
};

int main(int argc, char *argv[])
{
    if (argc != 7)
    {
        std::cerr << "Usage: " << argv[0] << " <numSets> <numBlocks> <blockSize> <writeAlloc> <writeBack> <lru>" << std::endl;
        return 1;
    }

    int numSets = std::stoi(argv[1]);
    int numBlocks = std::stoi(argv[2]);
    int blockSize = std::stoi(argv[3]);
    bool writeAlloc = std::string(argv[4]) == "write-allocate";
    bool writeBack = std::string(argv[5]) == "write-back";
    bool lru = std::string(argv[6]) == "lru";

    CacheSimulator sim(numSets, numBlocks, blockSize, writeAlloc, writeBack, lru);

    std::string line;
    while (getline(std::cin, line))
    {
        char type = line[0];
        unsigned long address = std::stoul(line.substr(2), nullptr, 16);
        sim.processAccess(type, address);
    }

    sim.printSummary();


    return 0;
}
