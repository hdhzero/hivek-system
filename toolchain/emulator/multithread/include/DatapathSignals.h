#ifndef HIVEK_MULTITHREAD_EMULATOR_DATAPATH_SIGNALS_H
#define HIVEK_MULTITHREAD_EMULATOR_DATAPATH_SIGNALS_H

#include "Register.h"
#include "ControlSignals.h"

namespace HivekMultithreadEmulator {
    class DatapathSignals {
        public:
            DatapathSignals();

        public:
            void generate_alu_res_for_lane(int lane);
            void generate_sh_res_for_lane(int lane);
            void generate_alu_sh_res_for_lane(int lane);

        private:
            u32 get_alu_first_operand(int lane);
            u32 get_alu_second_operand(int lane);
            u32 get_sh_ammount(int lane);

        private:
            ControlSignals* ctrl;
            Register<u32>* vra[N_LANES];
            Register<u32>* vrb[N_LANES];
            Register<u32>* immediate[N_LANES][3];
            Register<u32>* alu_res[N_LANES];
            Register<u32>* sh_res[N_LANES];
            Register<u32>* alu_sh_res[N_LANES];
            Register<u32>* mem_res[N_LANES];

    };
}

#endif
