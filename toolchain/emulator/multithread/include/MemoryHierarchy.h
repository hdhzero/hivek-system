#ifndef HIVEK_MULTITHREAD_EMULATOR_MEMORY_HIERARCHY_H
#define HIVEK_MULTITHREAD_EMULATOR_MEMORY_HIERARCHY_H

#include <vector>
#include <fstream>
#include "Defines.h"
#include "Register.h"
#include "RegisterPool.h"
#include "GPU.h"

namespace HivekMultithreadEmulator {
    class MemoryHierarchy {
        public:
            MemoryHierarchy();

        public:
            void reset();
            void cycle();
            void update();

        public:
            u64 iread64(int lane, u32 address, bool hits[8]);

        public:
            void dwrite16(int lane, u32 address, u16 data);
            void dwrite32(int lane, u32 address, u32 data);

        public:
            void read_contents_from_file(char* filename);

        private:
            Register<u32>* i_address[N_LANES];

        private:
            bool render;
            GPU gpu;

        private:
            RegisterPool rpool;
            std::vector<char> ram;
    };
}

#endif
