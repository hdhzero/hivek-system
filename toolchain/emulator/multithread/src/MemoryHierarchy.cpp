#include "../include/MemoryHierarchy.h"
using namespace HivekMultithreadEmulator;

void MemoryHierarchy::reset() {
    // TODO
}

void MemoryHierarchy::cycle() {
    // TODO
}

void MemoryHierarchy::update() {
    // TODO
}

void MemoryHierarchy::read_contents_from_file(char* filename) {
    std::ifstream file;
    char c;

    file.open(filename);

    while (file.get(c)) {
        ram.push_back(c);
    }

    file.close();
}
