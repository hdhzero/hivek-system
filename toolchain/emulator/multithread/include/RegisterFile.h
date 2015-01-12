#ifndef REGISTER_BANK_H
#define REGISTER_BANK_H

#include "Defines.h"
#include "Register.h"
#include "RegisterPool.h"

namespace HivekMultithreadEmulator {
    class RegisterFile {
        public:
            RegisterFile();

        public:
            void cycle();
            void update();

        public:
            void write(int lane, u32 thread, u32 wren, u32 rc, u32 vrc);
            void read_ra(int lane, u32 thread, u32 ra);
            void read_rb(int lane, u32 thread, u32 rb);
            u32 get_vra(int lane);
            u32 get_vrb(int lane);

        private:
            RegisterPool register_pool;

        private:
            u32 registers[16][32];
            Register<u32>* ra[N_LANES];
            Register<u32>* rb[N_LANES];
            Register<u32>* rc[N_LANES];
            Register<u32>* vrc[N_LANES];
            Register<u32>* wren[N_LANES];
            Register<u32>* thread_r[N_LANES];
            Register<u32>* thread_w[N_LANES];

    };
}

#endif
