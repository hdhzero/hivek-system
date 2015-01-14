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
            void generate_sh_controls(int lane, ControlTable* ct, u32 inst);
            void generate_reg_controls(int lane, ControlTable* ct, u32 inst);

        private:
            u32 control_address(u32 instruction);
            u32 extract_sh_immediate(u32 instruction);
            u32 extract_op_from_type_i(u32 instruction);
            u32 extract_op_from_type_iii(u32 instruction);
            u32 extract_ra(u32 instruction);
            u32 extract_rb(u32 instruction);
            u32 extract_rc(u32 instruction);
            u32 extract_sh_immd(u32 instruction);
            u32 extract_immd_from_type_i(u32 instruction);

            bool mask(u32 v, u32 m);
            bool is_type_i(u32 instruction);
            bool is_type_ii(u32 instruction); 
            bool is_type_iii(u32 instruction); 
            bool is_type_iv(u32 instruction);
            bool is_type_v(u32 instruction);
            int is_shadd(u32 instruction);

        private:
            class DatapathSignals* dpath;

        private:
            Register<u32>* ctrl_addr[N_LANES];

            Register<u32>* alu_op[N_LANES][2];
            Register<u32>* alu_pc_vra_sel[N_LANES][2];
            Register<u32>* alu_vrb_immediate_sel[N_LANES][2];
            Register<u32>* alu_sh_sel[N_LANES][3];
            Register<u32>* alu_sh_mem_sel[N_LANES][4];

            Register<u32>* sh_type[N_LANES][2];
            Register<u32>* sh_amount_sel[N_LANES][2];
            Register<u32>* sh_add[N_LANES][2];
            Register<u32>* sh_immediate[N_LANES][3];

            Register<u32>* ra;
            Register<u32>* rb[N_LANES][2];
            Register<u32>* rc[N_LANES][5];
            Register<u32>* r_wren[N_LANES][4];
            Register<u32>* r_dst_sel[N_LANES];
            Register<u32>* rb_31_sel[N_LANES];

            Register<u32>* p_wren[N_LANES][4];
            Register<u32>* predicate_value[N_LANES][3];
            Register<u32>* predicate_rvalue[N_LANES][2];
            Register<u32>* predicate_register[N_LANES][2];

            Register<u32>* m_wren[N_LANES][2];
            Register<u32>* m_size[N_LANES][2];
    };
}

#endif
