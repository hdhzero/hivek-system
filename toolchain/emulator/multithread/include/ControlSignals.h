#ifndef HIVEK_MULTITHREAD_EMULATOR_CONTROL_SIGNALS_H
#define HIVEK_MULTITHREAD_EMULATOR_CONTROL_SIGNALS_H

#include "Defines.h"
//#include "Utils.h"
#include "Register.h"
#include "DatapathSignals.h"

namespace HivekMultithreadEmulator {
    class ControlSignals {
        public:
            void generate_controls_for_lane(int lane);

        public:
            u32 get_alu_op(int lane);
            u32 get_alu_pc_vra_sel(int lane);
            u32 get_alu_vrb_immediate_sel(int lane);
            u32 get_alu_sh_sel(int lane);

            u32 get_sh_type(int lane);
            u32 get_sh_add(int lane);
            u32 get_sh_amount_sel(int lane);
            u32 get_sh_immediate(int lane);

        private:
            void generate_alu_controls(int lane, ControlTable* ct);
            void generate_sh_controls(int lane, ControlTable* ct);

        private:
            u32 extract_sh_immediate(u32 instruction);

        private:
            class DatapathSignals* dpath;

        private:
            Register<u32>* alu_op[N_LANES][2];
            Register<u32>* alu_pc_vra_sel[N_LANES][2];
            Register<u32>* alu_vrb_immediate_sel[N_LANES][2];
            Register<u32>* alu_sh_sel[N_LANES][3];
            Register<u32>* alu_sh_mem_sel[N_LANES][4];

            Register<u32>* sh_type[N_LANES][2];
            Register<u32>* sh_amount_sel[N_LANES][2];
            Register<u32>* sh_add[N_LANES][2];
            Register<u32>* sh_immediate[N_LANES][3];
    };
}

#endif
