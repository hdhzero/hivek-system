#ifndef REGISTER_BANK_H
#define REGISTER_BANK_H

#include "Defines.h"
#include "Register.h"
#include "RegisterPool.h"

namespace HivekMultithreadEmulator {
    class RegisterFile {
        public:
            void init();
            void set_rpool(RegisterPool* rpool);

        public:
            void write(int lane, u32 thread, u32 wren, u32 rc, u32 vrc);
            u32 read_ra(int lane, u32 thread, u32 ra);
            u32 read_rb(int lane, u32 thread, u32 rb);

        private:
            u32 get_vra(int lane);
            u32 get_vrb(int lane);

        private:
            RegisterPool* rpool;

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
