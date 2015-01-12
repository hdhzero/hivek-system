#ifndef HIVEK_MULTITHREAD_EMULATOR_HIVEK_H
#define HIVEK_MULTITHREAD_EMULATOR_HIVEK_H

#include "MemoryHierarchy.h"

namespace HivekMultithreadEmulator {
    class Hivek {
        public:
            void reset();
            void cycle();
            void update();

        public:
            void set_memory_hierarchy(MemoryHierarchy* ptr);

        private:
            void fetch();
            void expand();
            void decode();
            void execute();
            void writeback();

        private:
            MemoryHierarchy* mem;

        private:
    //        ControlSignals ctrl;
      //      DatapathSignals dpath;
        //    RegisterFile reg_file;
    };
}

#endif
