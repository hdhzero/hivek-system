#ifndef HIVEK_MULTITHREAD_EMULATOR_MEMORY_HIERARCHY_H
#define HIVEK_MULTITHREAD_EMULATOR_MEMORY_HIERARCHY_H

#include <vector>
#include <fstream>

namespace HivekMultithreadEmulator {
    class MemoryHierarchy {
        public:
            void reset();
            void cycle();
            void update();

        public:
            void read_contents_from_file(char* filename);

        private:
            std::vector<char> ram;
    };
}

#endif
