#ifndef HIVEK_MULTITHREAD_EMULATOR_DATAPATH_SIGNALS_H
#define HIVEK_MULTITHREAD_EMULATOR_DATAPATH_SIGNALS_H

#include "Register.h"
#include "RegisterPool.h"
#include "RegisterFile.h"
#include "ControlSignals.h"

namespace HivekMultithreadEmulator {
    class DatapathSignals {
        public:
            void init();
            void set_ctrl(class ControlSignals* ctrl);
            void set_rpool(class RegisterPool* rpool);
            void set_regfile(class RegisterFile* regfile);
            void set_mem(class MemoryHierarchy* mem);

        public:
            void generate_alu_res_for_lane(int lane);
            void generate_sh_res_for_lane(int lane);
            void generate_alu_sh_res_for_lane(int lane);
            u32 get_instruction_in_lane(int lane);

        private:
            u32 get_alu_first_operand(int lane);
            u32 get_alu_second_operand(int lane);
            u32 get_sh_amount(int lane);

        private:
            u32 alu(u32 op, u32 a, u32 b);
            u32 barrel_shifter(u32 shift_type, u32 a, u32 shift_ammount);

        private:
            class ControlSignals* ctrl;
            class RegisterPool* rpool;
            class RegisterFile* regfile;
            class MemoryHierarchy* mem;

        private:
            Register<u32>* primary_thread[2];
            Register<u32>* pc[16];
            Register<u32>* pcs[N_LANES][7];
            Register<u64>* rt_instructions;
            Register<u64>* nrt_instructions;
            Register<u32>* thread[N_LANES][6];

            Register<u32>* instruction[N_LANES];
            Register<u32>* instruction_size[N_LANES][5];
            Register<u32>* instruction_kind[N_LANES][3];
            Register<u32>* instruction_rtk[N_LANES][5];

            Register<u32>* vra[N_LANES][2];
            Register<u32>* vrb[N_LANES][2];
            Register<u32>* immediate[N_LANES][3];

            Register<u32>* alu_res[N_LANES];
            Register<u32>* sh_res[N_LANES];

            Register<u32>* alu_sh_res[N_LANES];
            Register<u32>* mem_res[N_LANES];
    };
}

#endif
