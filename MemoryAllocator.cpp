#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <dirent.h> // For reading files in a directory

using namespace std;

struct MemoryBlock {
    int start;
    int size;
    bool isFree;
    string process;

    MemoryBlock(int start, int size) : start(start), size(size), isFree(true), process("") {}
};

class MemoryAllocator {
private:
    vector<MemoryBlock> memoryBlocks;

    void mergeFreeMemoryBlocks() {
        for (size_t i = 0; i < memoryBlocks.size() - 1; ++i) {
            if (memoryBlocks[i].isFree && memoryBlocks[i + 1].isFree) {
                memoryBlocks[i].size += memoryBlocks[i + 1].size;
                memoryBlocks.erase(memoryBlocks.begin() + i + 1);
                --i;
            }
        }
    }

public:
    MemoryAllocator(int totalMemory) {
        memoryBlocks.push_back(MemoryBlock(0, totalMemory));
    }

    // ************************************************************************************************
    void allocateMemory(const string& process, int size, const string& strategy) {
        MemoryBlock* selectedBlock = nullptr;

        if (strategy == "F") {
            for (size_t i = 0; i < memoryBlocks.size(); ++i) {
                if (memoryBlocks[i].isFree && memoryBlocks[i].size >= size) {
                    selectedBlock = &memoryBlocks[i];
                    break;
                }
            }
        } else if (strategy == "B") {
            for (size_t i = 0; i < memoryBlocks.size(); ++i) {
                if (memoryBlocks[i].isFree && memoryBlocks[i].size >= size) {
                    if (!selectedBlock || memoryBlocks[i].size < selectedBlock->size) {
                        selectedBlock = &memoryBlocks[i];
                    }
                }
            }
        } else if (strategy == "W") { 
            for (size_t i = 0; i < memoryBlocks.size(); ++i) {
                if (memoryBlocks[i].isFree && memoryBlocks[i].size >= size) {
                    if (!selectedBlock || memoryBlocks[i].size > selectedBlock->size) {
                        selectedBlock = &memoryBlocks[i];
                    }
                }
            }
        } else {
            cout << "Error: Invalid allocation strategy.\n";
            return;
        }

        if (!selectedBlock) {
            cout << "Error: Not enough memory for process " << process << ".\n";
            return;
        }

        selectedBlock->isFree = false;
        selectedBlock->process = process;

        if (selectedBlock->size > size) {
            MemoryBlock newBlock(selectedBlock->start + size, selectedBlock->size - size);
            selectedBlock->size = size;
            int index = 0;
            for (size_t i = 0; i < memoryBlocks.size(); ++i) {
                if (&memoryBlocks[i] == selectedBlock) {
                    index = i;
                    break;
                }
            }
            memoryBlocks.insert(memoryBlocks.begin() + index + 1, newBlock);
        }
    }

    // ************************************************************************************************
    void releaseMemory(const string& process) {
        bool found = false;
        for (size_t i = 0; i < memoryBlocks.size(); ++i) {
            if (!memoryBlocks[i].isFree && memoryBlocks[i].process == process) {
                memoryBlocks[i].isFree = true;
                memoryBlocks[i].process = "";
                found = true;
            }
        }
        if (!found) {
            cout << "Error: Process " << process << " not found.\n";
        } else {
            mergeFreeMemoryBlocks();
        }
    }

    // ************************************************************************************************
    void compactMemory() {
        int freeSize = 0;
        int currentAddress = 0;
        vector<MemoryBlock> compactedBlocks;

        for (size_t i = 0; i < memoryBlocks.size(); ++i) {
            if (!memoryBlocks[i].isFree) {
                memoryBlocks[i].start = currentAddress;
                currentAddress += memoryBlocks[i].size;
                compactedBlocks.push_back(memoryBlocks[i]);
            } else {
                freeSize += memoryBlocks[i].size;
            }
        }

        if (freeSize > 0) {
            compactedBlocks.push_back(MemoryBlock(currentAddress, freeSize));
        }

        memoryBlocks = compactedBlocks;
    }

    // ************************************************************************************************
    void printStats() const {
        cout << "Current memory status:\n";
        for (size_t i = 0; i < memoryBlocks.size(); ++i) {
            if (memoryBlocks[i].isFree) {
                cout << "  Addresses [" << memoryBlocks[i].start << ":" << (memoryBlocks[i].start + memoryBlocks[i].size - 1) << "] Unused\n";
            } else {
                cout << "  Addresses [" << memoryBlocks[i].start << ":" << (memoryBlocks[i].start + memoryBlocks[i].size - 1) << "] Process " << memoryBlocks[i].process << "\n";
            }
        }
        cout << endl;
    }
};

// ************************************************************************************************
void processFile(const string& filePath) {
    ifstream inputFile(filePath);
    if (!inputFile) {
        cerr << "Error: Unable to open input file " << filePath << ".\n";
        return;
    }

    string line;
    getline(inputFile, line);
    int totalMemory = stoi(line);
    MemoryAllocator allocator(totalMemory);

    while (getline(inputFile, line)) {
        istringstream iss(line);
        string command;
        iss >> command;

        if (command == "RQ") {
            string process, strategy;
            int size;
            iss >> process >> size >> strategy;
            allocator.allocateMemory(process, size, strategy);
        } else if (command == "RL") {
            string process;
            iss >> process;
            allocator.releaseMemory(process);
        } else if (command == "C") {
            allocator.compactMemory();
        } else if (command == "STAT") {
            allocator.printStats();
        } else {
            cout << "Error: Invalid command.\n";
        }
    }

    inputFile.close();
}

// ************************************************************************************************
int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: ./MemoryAllocator file_name" << endl;
        return 1;
    }

    string filePath = argv[1];
    cout << "Processing file: " << filePath << "\n";
    processFile(filePath);

    return 0;
}

/* 
  Compile the code
    g++ -o MemoryAllocator MemoryAllocator.cpp

  Run the code
  ./MemoryAllocator file_name
*/