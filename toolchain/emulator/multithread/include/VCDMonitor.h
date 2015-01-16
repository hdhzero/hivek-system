#ifndef HIVEK_MULTITHREAD_EMULATOR_VCD_MONITOR_H
#define HIVEK_MULTITHREAD_EMULATOR_VCD_MONITOR_H

#include "Defines.h"
#include "Register.h"
#include <vector>
#include <fstream>

namespace HivekMultithreadEmulator {
    class VCDMonitor {
        public:
            void open(std::string filename);
            void close();
            void set_module_name(std::string name);
            void add_register(Register<u32>* ptr);
            void add_register64(Register<u64>* ptr);
            void create_vcd_header();
            void dump_signals(int timestamp);
            
        private:
            std::ofstream waves;
            std::string module_name;
            std::vector<Register<u32>* > regs;
            std::vector<Register<u64>* > regs64;
    };
}

#endif
