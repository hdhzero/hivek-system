#ifndef HIVEK_MULTITHREAD_EMULATOR_HIVEK_H
#define HIVEK_MULTITHREAD_EMULATOR_HIVEK_H

#include "MemoryHierarchy.h"
#include "VCDMonitor.h"
#include "RegisterFile.h"

namespace HivekMultithreadEmulator {
    class Hivek {
        public:
            Hivek();

        public:
            void reset();
            void cycle();
            void update();

        public:
            void set_memory_hierarchy(MemoryHierarchy* ptr);
            void add_waves_to_vcd(VCDMonitor* ptr);

        private:
            void fetch();
            void expand();
            void decode();
            void execute();
            void writeback();

        private:
            void generate_controls_for_lane(int lane);
            void generate_alu_controls(int lane, ControlTable* ct);
            void generate_sh_controls(int lane, ControlTable* ct, u32 inst);
            void generate_reg_controls(int lane, ControlTable* ct, u32 inst);
            void generate_pred_controls(int lane, ControlTable* ct, u32 inst);
            void generate_mem_controls(int lane, ControlTable* ct);

            void generate_alu_res_for_lane(int lane);
            void generate_sh_res_for_lane(int lane);
            void generate_alu_sh_res_for_lane(int lane);
            void writeback_lane(int lane);

            void read_registers_in_lane(int lane);
            void calculate_next_rt_pc();
            void calculate_next_nrt_pc();

        private:
            u32 alu(u32 op, u32 a, u32 b);
            u32 barrel_shifter(u32 shift_type, u32 a, u32 shift_ammount);

        private:
            u32 control_address(u32 instruction);
            u32 extract_sh_immediate(u32 instruction);
            u32 extract_op_from_type_i(u32 instruction);
            u32 extract_op_from_type_iii(u32 instruction);
            u32 extract_ra(u32 instruction);
            u32 extract_rb(u32 instruction);
            u32 extract_rc(u32 instruction);
            u32 extract_predicate_value(u32 instruction);
            u32 extract_predicate_register(u32 instruction);
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
            RegisterPool rpool;

        private:
            MemoryHierarchy* mem;
            RegisterFile regfile;

        private:
            Register<u32>* ctrl_addr[N_LANES];
            Register<u32>* primary_thread[2];
            Register<u32>* pc[16];
            Register<u32>* pcs[N_LANES][7];
            Register<u64>* rt_instructions;
            Register<u64>* nrt_instructions;
            Register<u32>* threads[N_LANES][6];

            Register<u32>* instructions[N_LANES];
            Register<u32>* instruction_size[N_LANES][5];
            Register<u32>* instruction_kind[N_LANES][3];
            Register<u32>* instruction_rtk[5];

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
            Register<u32>* p_value[N_LANES][3];
            Register<u32>* p_rvalue[N_LANES][2];
            Register<u32>* p_register[N_LANES][2];

            Register<u32>* m_wren[N_LANES][2];
            Register<u32>* m_size[N_LANES][2];

        private:
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
