#include "include/Hivek.h"
#include "include/MemoryHierarchy.h"

using namespace HivekMultithreadEmulator;

int main(int argc, char** argv) {
    Hivek hivek;
    MemoryHierarchy mem;

    hivek.set_memory_hierarchy(&mem);
    mem.read_contents_from_file(argv[1]);
    
    hivek.reset();
    mem.reset();

    for (int i = 0; i < 1000; ++i) {
        hivek.cycle();
        mem.cycle();
        hivek.update();
        mem.update();
    }

    return 0;
}
