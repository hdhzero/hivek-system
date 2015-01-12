#ifndef REGPOOL_H
#define REGPOOL_H

#include <vector>
#include <fstream>
#include "Defines.h"
#include "Register.h"

namespace HivekMultithreadEmulator {
    class RegisterPool {
        private:
            std::vector<Register<u32>* > register_pool;
            std::vector<Register<u64>* > register_pool_64;
            std::vector<Register<u32>* > vcd_registers;
            std::vector<Register<u64>* > vcd_registers_64;

        public:
            ~RegisterPool();

        public:
            void reset();
            void update();

        public:
            Register<u32>* create_register(std::string name, int width);
            Register<u64>* create_register64(std::string name, int width);
    };
}

#endif
