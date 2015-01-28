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
            void dump_registers();

        private:
            void fetch();
            void expand();
            void decode();
            void execute();
            void writeback();

        private:
            void get_instructions_from_rt();
            void get_instructions_from_nrt();
            void generate_threads();
            void generate_instruction_sizes();
            void generate_rtks();
            void generate_immediates(int lane, u32 inst);

            void generate_controls_for_lane(int lane);
            void generate_alu_controls(int lane, ControlTable* ct);
            void generate_sh_controls(int lane, ControlTable* ct, u32 inst);
            void generate_reg_controls(int lane, ControlTable* ct, u32 inst);
            void generate_pred_controls(int lane, ControlTable* ct, u32 inst);
            void generate_mem_controls(int lane, ControlTable* ct);

            void generate_alu_res_for_lane(int lane);
            void generate_sh_res_for_lane(int lane);
            void generate_alu_sh_res_for_lane(int lane);
            void generate_jump_res_for_lane(int lane);
            void writeback_lane(int lane);

            void read_registers_in_lane(int lane);
            void access_dmem(int lane);
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
            u32 extract_immd_from_type_ii(u32 instruction);
            u32 extract_immd_from_type_iv(u32 instruction);
            u32 extract_immd_from_type_v(u32 instruction);

            bool mask(u32 v, u32 m);
            bool is_type_i(u32 instruction);
            bool is_type_ii(u32 instruction); 
            bool is_type_iii(u32 instruction); 
            bool is_type_iv(u32 instruction);
            bool is_type_v(u32 instruction);
            bool parallel(u64 instruction);
            int is_shadd(u32 instruction);
            u32 get_first_instruction(u64 instruction, int& size);
            u32 get_second_instruction(u64 instruction, int& size);
            u32 expand16(u32 instruction);
            u32 expand24(u32 instruction);
            u32 encode_instruction_size(u32 size);
            u32 decode_instruction_size(u32 size);
            RTNextPCSels rtnp_make(u32 pc_alu_sel, u32 alu_sel, u32 zero_sz_sel, u32 sz_sel);
            RTNextPCSels next_rt_pc_rt_rt(u32 k0, u32 k1, u32 p0, u32 p1);
            RTNextPCSels next_rt_pc_rt_nrt(u32 k0, u32 p0);
            RTNextPCSels next_rt_pc_rt_nop(u32 k0, u32 p0);
            RTNextPCSels rt_next_pc(u32 rtk, u32 k0, u32 k1, u32 p0, u32 p1);

        private:
            RegisterPool rpool;

        private:
            MemoryHierarchy* mem;
            RegisterFile regfile;

        private:
            Register<u32>* ctrl_addr[N_LANES];
            Register<u32>* primary_thread[3];
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
            Register<u32>* alu_sh_mem_jump_sel[N_LANES][4];

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
            Register<u32>* p_register;
            Register<u32>* pres[N_LANES];

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
            Register<u32>* jump_res[N_LANES];
    };
}

#endif
