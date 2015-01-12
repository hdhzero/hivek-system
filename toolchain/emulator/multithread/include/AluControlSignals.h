#ifndef HIVEK_MULTITHREAD_EMULATOR_ALU_CONTROL_SIGNALS_H
#define HIVEK_MULTITHREAD_EMULATOR_ALU_CONTROL_SIGNALS_H

#include "Defines.h"
#include "Register.h"

namespace HivekMultithreadEmulator {
    class AluControlSignals {
        public:
            void generate_lane(int lane, ControlTable* ct);

        private:
            Register<u32>* alu_op[N_LANES][2];
            Register<u32>* alu_src[N_LANES][2];
            Register<u32>* alu_sh_src[N_LANES][3];
            Register<u32>* alu_sh_mem_src[N_LANES][4];
    };
}

#endif
