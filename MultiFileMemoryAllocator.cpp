#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>


using namespace std;
namespace fs = std::filesystem;

struct MemoryBlock {
    int start;
    int size;
    bool isFree;
    string process;

    MemoryBlock(int start, int size) : start(start), size(size), isFree(true), process("") {}

    bool operator==(const MemoryBlock& other) const {
        return start == other.start && size == other.size && isFree == other.isFree && process == other.process;
    }
};

class MemoryAllocator {
private:
    vector<MemoryBlock> memoryBlocks;

    // ************************************************************************************************
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
            auto it = std::find_if(memoryBlocks.begin(), memoryBlocks.end(),[&selectedBlock](const MemoryBlock& block) {
                           return block == *selectedBlock; // Compare memory blocks
            });
            memoryBlocks.insert(it + 1, newBlock);
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
    void printStats(const string& outputFileName) const {
        ofstream outputFile(outputFileName, ios::app);
        if (!outputFile) {
            cerr << "Error: Unable to open output file " << outputFileName << ".\n";
            return;
        }

        cout << "Current memory status:\n";
        outputFile << "Current memory status:\n";

        for (size_t i = 0; i < memoryBlocks.size(); ++i) {
            if (memoryBlocks[i].isFree) {
                cout << "  Addresses [" << memoryBlocks[i].start << ":" << (memoryBlocks[i].start + memoryBlocks[i].size - 1) << "] Unused\n";
                outputFile << "  Addresses [" << memoryBlocks[i].start << ":" << (memoryBlocks[i].start + memoryBlocks[i].size - 1) << "] Unused\n";
            } else {
                cout << "  Addresses [" << memoryBlocks[i].start << ":" << (memoryBlocks[i].start + memoryBlocks[i].size - 1) << "] Process " << memoryBlocks[i].process << "\n";
                outputFile << "  Addresses [" << memoryBlocks[i].start << ":" << (memoryBlocks[i].start + memoryBlocks[i].size - 1) << "] Process " << memoryBlocks[i].process << "\n";
            }
        }

        cout << endl;
        outputFile << endl;
        outputFile.close();
    }
};

// ************************************************************************************************
void processFile(const string& inputFileName, const string& outputFileName) {
    ifstream inputFile(inputFileName);
    if (!inputFile) {
        cerr << "Error: Unable to open input file " << inputFileName << ".\n";
        return;
    }

    // Append a header for this file's results
    ofstream outputFile(outputFileName, ios::app);
    if (!outputFile) {
        cerr << "Error: Unable to open output file " << outputFileName << ".\n";
        return;
    }
    outputFile << "Processing file: " << inputFileName << "\n";
    outputFile.close();

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
            allocator.printStats(outputFileName);
        } else {
            cout << "Error: Invalid command.\n";
        }
    }

    inputFile.close();
}


// ************************************************************************************************
int main() {
    string inputFolder = "inputs";
    string outputFileName = "memory_stats.txt";

    // Clear the output file at the start of the program
    ofstream clearFile(outputFileName, ios::trunc);
    clearFile.close();

    // Process each file in the input folder
    for (const auto& entry : fs::directory_iterator(inputFolder)) {
        if (entry.is_regular_file()) {
            cout << "Processing file: " << entry.path().filename() << endl;
            processFile(entry.path().string(), outputFileName);
        }
    }

    cout << "All outputs stored in " << outputFileName << endl;
    return 0;
}


// ************************************************************************************************
/* 
  Compile the code:
    g++ -std=c++17 -o MultiFileMemoryAllocator MultiFileMemoryAllocator.cpp

  Run the code:
    ./MultiFileMemoryAllocator
*/
