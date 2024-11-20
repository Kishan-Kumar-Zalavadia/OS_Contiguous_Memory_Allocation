#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>

class MemoryBlock {
public:
    int start;
    int size;
    std::string process;

    MemoryBlock(int start, int size, std::string process = "")
        : start(start), size(size), process(process) {}

    int getEnd() const { return start + size - 1; }

    // Add equality operator for std::find
    bool operator==(const MemoryBlock& other) const {
        return start == other.start && size == other.size && process == other.process;
    }
};

class MemoryManager {
private:
    int totalSize;
    std::vector<MemoryBlock> blocks;

    MemoryBlock* findHole(int size, char strategy) {
        std::vector<MemoryBlock*> suitableHoles;
        for (std::vector<MemoryBlock>::iterator it = blocks.begin(); it != blocks.end(); ++it) {
            if (it->process.empty() && it->size >= size) {
                suitableHoles.push_back(&(*it));
            }
        }

        if (suitableHoles.empty()) return nullptr;

        switch (strategy) {
            case 'F': // First Fit
                return suitableHoles[0];
            case 'B': // Best Fit
                return *std::min_element(suitableHoles.begin(), suitableHoles.end(),
                    [](const MemoryBlock* a, const MemoryBlock* b) {
                        return a->size < b->size;
                    });
            case 'W': // Worst Fit
                return *std::max_element(suitableHoles.begin(), suitableHoles.end(),
                    [](const MemoryBlock* a, const MemoryBlock* b) {
                        return a->size < b->size;
                    });
            default:
                throw std::invalid_argument("Unknown strategy");
        }
    }

    void mergeAdjacentHoles() {
        std::vector<MemoryBlock>::iterator it = blocks.begin();
        while (it != blocks.end() - 1) {
            if (it->process.empty() && (it + 1)->process.empty()) {
                it->size += (it + 1)->size;
                it = blocks.erase(it + 1);
            } else {
                ++it;
            }
        }
    }

public:
    MemoryManager(int totalSize) : totalSize(totalSize) {
        blocks.push_back(MemoryBlock(0, totalSize));
    }

    bool requestMemory(const std::string& process, int size, char strategy) {
        MemoryBlock* hole = findHole(size, strategy);
        if (!hole) {
            std::cout << "Insufficient memory\n";
            return false;
        }

        if (hole->size > size) {
            MemoryBlock newBlock(hole->start, size, process);
            MemoryBlock remainingHole(hole->start + size, hole->size - size);

            std::vector<MemoryBlock>::iterator it = 
                std::find(blocks.begin(), blocks.end(), *hole);
            *it = newBlock;
            blocks.insert(it + 1, remainingHole);
        } else {
            hole->process = process;
        }

        return true;
    }

    bool releaseMemory(const std::string& process) {
        bool found = false;
        for (std::vector<MemoryBlock>::iterator it = blocks.begin(); 
             it != blocks.end(); ++it) {
            if (it->process == process) {
                it->process = "";
                found = true;
            }
        }

        if (found) {
            mergeAdjacentHoles();
            return true;
        }
        
        std::cout << "Process not found\n";
        return false;
    }

    void compact() {
        std::vector<MemoryBlock> allocatedBlocks;
        for (std::vector<MemoryBlock>::iterator it = blocks.begin(); 
             it != blocks.end(); ++it) {
            if (!it->process.empty()) {
                allocatedBlocks.push_back(*it);
            }
        }

        blocks.clear();
        int currentAddress = 0;
        for (std::vector<MemoryBlock>::iterator it = allocatedBlocks.begin(); 
             it != allocatedBlocks.end(); ++it) {
            it->start = currentAddress;
            currentAddress += it->size;
            blocks.push_back(*it);
        }

        if (currentAddress < totalSize) {
            blocks.push_back(MemoryBlock(currentAddress, totalSize - currentAddress));
        }
    }

    void status() {
        for (std::vector<MemoryBlock>::const_iterator it = blocks.begin(); 
             it != blocks.end(); ++it) {
            std::cout << "Addresses [" << it->start << ":" << it->getEnd() 
                      << "] " << (it->process.empty() ? "Unused" : "Process " + it->process) 
                      << std::endl;
        }
    }

    static void processCommands(const std::string& filename) {
        std::ifstream file(filename.c_str());
        if (!file) {
            std::cerr << "Unable to open file\n";
            return;
        }

        std::string line;
        std::getline(file, line);
        int totalMemory = atoi(line.c_str());
        MemoryManager manager(totalMemory);

        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string command;
            iss >> command;

            if (command == "RQ") {
                std::string process;
                int size;
                char strategy;
                iss >> process >> size >> strategy;
                manager.requestMemory(process, size, strategy);
            } 
            else if (command == "RL") {
                std::string process;
                iss >> process;
                manager.releaseMemory(process);
            } 
            else if (command == "C") {
                manager.compact();
            } 
            else if (command == "STAT") {
                std::cout << "\nCurrent memory status:\n";
                manager.status();
            }
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }

    try {
        MemoryManager::processCommands(argv[1]);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}