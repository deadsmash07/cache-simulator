#include <iostream>
#include <vector>
#include <string>

// Define cache line structure
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
    CacheSimulator(int numSets, int numBlocks, int blockSize, bool writeAlloc, bool writeBack, bool lru)
        : sets(numSets), blocksPerSet(numBlocks), blockSize(blockSize),
          writeAllocate(writeAlloc), writeBack(writeBack), useLRU(lru)
    {
        // Initialize each set
        cache.resize(sets, std::vector<CacheLine>(blocksPerSet));
        loadHits = loadMisses = storeHits = storeMisses = totalCycles = 0;
    }

    // Process a memory access
    void processAccess(char type, unsigned long address)
    {
        unsigned long blockAddress = address / blockSize;
        unsigned long setIndex = blockAddress % sets;
        unsigned long tag = blockAddress / sets;

        bool hit = false;
        int freeIndex = -1;
        int lruIndex = 0; // Index for least recently used block

        // Check all blocks in the set for a hit
        for (int i = 0; i < blocksPerSet; ++i)
        {
            if (cache[setIndex][i].valid && cache[setIndex][i].tag == tag)
            {
                hit = true;
                if (useLRU)
                {
                    cache[setIndex][i].lastUsed = totalCycles;
                }
                if (type == 's')
                {
                    if (writeBack)
                    {
                        cache[setIndex][i].dirty = true;
                    }
                    else
                    { // If write-through, also "write to memory"
                        totalCycles += memoryAccessCycles;
                    }
                }
                break;
            }
            if (!cache[setIndex][i].valid && freeIndex == -1)
            {
                freeIndex = i; // Save first free block index
            }
            if (useLRU && cache[setIndex][lruIndex].lastUsed > cache[setIndex][i].lastUsed)
            {
                lruIndex = i;
            }
        }

        // Miss handling
        if (!hit)
        {
            if (type == 'l' || (type == 's' && writeAllocate))
            {
                int replaceIndex = (freeIndex != -1) ? freeIndex : lruIndex;
                if (cache[setIndex][replaceIndex].valid && writeBack && cache[setIndex][replaceIndex].dirty)
                {
                    // Write back dirty block to memory
                    totalCycles += memoryAccessCycles;
                }
                cache[setIndex][replaceIndex].valid = true;
                cache[setIndex][replaceIndex].tag = tag;
                cache[setIndex][replaceIndex].dirty = (type == 's');
                if (useLRU)
                {
                    cache[setIndex][replaceIndex].lastUsed = totalCycles;
                }
                totalCycles += memoryAccessCycles; // Memory access for loading the block
            }
            else if (type == 's')
            {
                // Write directly to memory (no-write-allocate)
                totalCycles += memoryAccessCycles;
            }
        }

        // Update the statistics
        updateStatistics(type, hit);
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
    }

private:
    void updateStatistics(char type, bool hit)
    {
        if (type == 'l')
        {
            if (hit)
            {
                loadHits++;
            }
            else
            {
                loadMisses++;
            }
        }
        else if (type == 's')
        {
            if (hit)
            {
                storeHits++;
            }
            else
            {
                storeMisses++;
            }
        }
        // Assuming 1 cycle per cache access, regardless of hit or miss.
        totalCycles += 1;
    }

    int sets, blocksPerSet, blockSize;
    bool writeAllocate, writeBack, useLRU;
    std::vector<std::vector<CacheLine>> cache;
    int loadHits, loadMisses, storeHits, storeMisses, totalCycles;
    const int memoryAccessCycles = 100; // Assumed penalty for memory access on misses
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
