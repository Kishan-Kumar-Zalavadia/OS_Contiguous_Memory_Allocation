#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

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
    // Allocate the memory for the process
    void allocateMemory(const string& process, int size, const string& strategy) {
        MemoryBlock* selectedBlock = nullptr;

        // First Fit
        if (strategy == "F") {
            for (size_t i = 0; i < memoryBlocks.size(); ++i) {
                if (memoryBlocks[i].isFree && memoryBlocks[i].size >= size) {
                    selectedBlock = &memoryBlocks[i];
                    break;
                }
            }
        // Best Fit
        } else if (strategy == "B") {
            for (size_t i = 0; i < memoryBlocks.size(); ++i) {
                if (memoryBlocks[i].isFree && memoryBlocks[i].size >= size) {
                    if (!selectedBlock || memoryBlocks[i].size < selectedBlock->size) {
                        selectedBlock = &memoryBlocks[i];
                    }
                }
            }
        // Worst Fit
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

        // Memory allocation
        selectedBlock->isFree = false;
        selectedBlock->process = process;

        if (selectedBlock->size > size) {
            // Split the memory block into two
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
    // Releases the memory for the process
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
    // Compact the memory blocks
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
    // Display the current status of the memory blocks
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

void processFile(const string& inputFileName) {
    ifstream inputFile(inputFileName);
    if (!inputFile) {
        cerr << "Error: Unable to open input file " << inputFileName << ".\n";
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

int main() {
    string inputFolder = "inputs";  // Folder containing input files
    for (const auto& entry : fs::directory_iterator(inputFolder)) {
        if (entry.is_regular_file()) {
            cout << "Processing file: " << entry.path().filename() << endl;
            processFile(entry.path().string());
        }
    }

    return 0;
}

/* 
  Compile the code
  g++ -std=c++17 -o MultiFileMemoryAllocator MultiFileMemoryAllocator.cpp                                                  ─╯
*/