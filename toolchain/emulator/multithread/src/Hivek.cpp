#include "../include/Hivek.h"
#include <sstream>
#include <iostream>
using namespace HivekMultithreadEmulator;

void Hivek::reset() {
    rpool.reset();
}

void Hivek::cycle() {
    fetch();
    expand();
    decode();
    execute();
    writeback();
}

void Hivek::update() {
    rpool.update();
}

void Hivek::set_memory_hierarchy(MemoryHierarchy* ptr) {
    mem = ptr;
}

void Hivek::fetch() {
    // TODO
}

void Hivek::expand() {
    generate_threads();
}

void Hivek::decode() {
    generate_controls_for_lane(0);
    generate_controls_for_lane(1);
    read_registers_in_lane(0);
    read_registers_in_lane(1);
}

void Hivek::execute() {
    generate_alu_res_for_lane(0);
    generate_sh_res_for_lane(0);
    generate_alu_sh_res_for_lane(0);
    calculate_next_rt_pc();

    generate_alu_res_for_lane(1);
    generate_sh_res_for_lane(1);
    generate_alu_sh_res_for_lane(1);
    calculate_next_nrt_pc();
}

void Hivek::writeback() {
    // TODO
    writeback_lane(0);
    writeback_lane(1);
}

void Hivek::calculate_next_rt_pc() {
    // TODO
}

void Hivek::calculate_next_nrt_pc() {
    // TODO
}

void Hivek::read_registers_in_lane(int lane) {
    u32 inst = instructions[lane]->read();
    u32 th;
    u32 ra;
    u32 rb;
    u32 pr;

    if (lane == 0) {
        th = this->threads[lane][0]->read();
        ra = extract_ra(inst);
        rb = extract_rb(inst);

        vra[lane][0]->write(regfile.read_ra(lane, th, ra));
        vrb[lane][0]->write(regfile.read_rb(lane, th, rb));

        vra[lane][1]->write(vra[lane][0]->read());       
        vrb[lane][1]->write(vrb[lane][0]->read());

        pr = extract_predicate_register(inst);
        p_rvalue[lane][0]->write(regfile.read_pr(lane, pr));
        p_rvalue[lane][1]->write(p_rvalue[lane][0]->read());
    } else {
        th = this->threads[lane][1]->read();
        ra = this->ra->read();
        rb = this->rb[lane][0]->read();

        vra[lane][1]->write(regfile.read_ra(lane, th, ra));
        vrb[lane][1]->write(regfile.read_rb(lane, th, rb));
    }
}

void Hivek::generate_controls_for_lane(int lane) {
    ControlTable* ct;
    u32 inst;

    inst = instructions[lane]->read(); 
    ctrl_addr[lane]->write(control_address(inst));
    ct = &control_table[ctrl_addr[lane]->read()];

    generate_alu_controls(lane, ct);
    generate_sh_controls(lane, ct, inst);
    generate_reg_controls(lane, ct, inst);
    generate_pred_controls(lane, ct, inst);
    generate_immediates(lane, inst);
    // TODO
}

void Hivek::generate_threads() {
    for (int lane = 0; lane < N_LANES; ++lane) {
        for (int i = 0; i < 5; ++i) {
            threads[lane][i + 1]->write(threads[lane][i]->read());
        }
    }
}

void Hivek::generate_instruction_sizes() {
    for (int lane = 0; lane < N_LANES; ++lane) {
        for (int i = 0; i < 4; ++i) {
            instruction_size[lane][i + 1]->write(instruction_size[lane][i]->read());
        }
    }
}

void Hivek::generate_rtks() {
    for (int i = 0; i < 4; ++i) {
        instruction_rtk[i + 1]->write(instruction_rtk[i]->read());
    }
}

void Hivek::generate_immediates(int lane, u32 inst) {
    u32 tmp = 0;

    if (is_type_i(inst)) {
        tmp = extract_immd_from_type_i(inst);

        if (tmp & 0x200) {
            tmp = 0x0FFFFFc00 | tmp;
        }
    } else if (is_type_ii(inst)) {
        tmp = extract_immd_from_type_ii(inst);

        if (tmp & 0x01000000) {
            tmp = 0x0FE000000 | tmp;
        }
    } else if (is_type_iv(inst)) {
        tmp = extract_immd_from_type_iv(inst);

        if (tmp & 0x08000) {
            tmp = 0x0FFFF0000 | tmp;
        }
    }

    immediate[lane][0]->write(tmp);
    immediate[lane][1]->write(immediate[lane][0]->read());
    immediate[lane][2]->write(immediate[lane][1]->read());
}

void Hivek::generate_alu_controls(int lane, ControlTable* ct) {
    alu_op[lane][0]->write(ct->alu_op);
    alu_op[lane][1]->write(alu_op[lane][0]->read());

    alu_pc_vra_sel[lane][0]->write(ct->alu_pc_vra_sel);
    alu_pc_vra_sel[lane][1]->write(alu_pc_vra_sel[lane][0]->read());

    alu_vrb_immediate_sel[lane][0]->write(ct->alu_vrb_immediate_sel);
    alu_vrb_immediate_sel[lane][1]->write(alu_vrb_immediate_sel[lane][0]->read());

    alu_sh_sel[lane][0]->write(ct->alu_sh_sel);
    alu_sh_sel[lane][1]->write(alu_sh_sel[lane][0]->read());
    alu_sh_sel[lane][2]->write(alu_sh_sel[lane][1]->read());

    alu_sh_mem_sel[lane][0]->write(ct->alu_sh_mem_sel);
    alu_sh_mem_sel[lane][1]->write(alu_sh_mem_sel[lane][0]->read());
    alu_sh_mem_sel[lane][2]->write(alu_sh_mem_sel[lane][1]->read());
    alu_sh_mem_sel[lane][3]->write(alu_sh_mem_sel[lane][2]->read());
}

void Hivek::generate_sh_controls(int lane, ControlTable* ct, u32 inst) {
    sh_type[lane][0]->write(ct->sh_type);
    sh_type[lane][1]->write(sh_type[lane][0]->read());

    sh_amount_sel[lane][0]->write(ct->sh_amount_sel);
    sh_amount_sel[lane][1]->write(sh_amount_sel[lane][0]->read());

    sh_add[lane][0]->write(ct->sh_add);
    sh_add[lane][1]->write(sh_add[lane][0]->read());

    sh_immediate[lane][0]->write(extract_sh_immediate(inst));
    sh_immediate[lane][1]->write(sh_immediate[lane][0]->read());
    sh_immediate[lane][2]->write(sh_immediate[lane][1]->read());
}

void Hivek::generate_reg_controls(int lane, ControlTable* ct, u32 inst) {
    r_dst_sel[lane]->write(ct->r_dst_sel);
    rb_31_sel[lane]->write(ct->rb_31_sel);

    r_wren[lane][0]->write(ct->r_wren);
    r_wren[lane][1]->write(r_wren[lane][0]->read());
    r_wren[lane][2]->write(r_wren[lane][1]->read());
    r_wren[lane][3]->write(r_wren[lane][2]->read());

    rb[lane][0]->write(extract_rb(inst));
    rb[lane][1]->write(rb[lane][0]->read());

    if (lane == 1) {
        ra->write(extract_ra(inst));
    }
   
    rc[lane][0]->write(extract_rc(inst));
    rc[lane][1]->write(rc[lane][0]->read());
    rc[lane][2]->write(rc[lane][1]->read());
    rc[lane][3]->write(rc[lane][2]->read());
    rc[lane][4]->write(rc[lane][3]->read());

    if (r_dst_sel[lane]->read()) {
        if (rb_31_sel[lane]->read()) {
            rc[lane][2]->write(31);
        } else {
            rc[lane][2]->write(rb[lane][1]->read());
        }
    }
}

void Hivek::generate_pred_controls(int lane, ControlTable* ct, u32 inst) {
    p_wren[lane][0]->write(ct->p_wren);
    p_wren[lane][1]->write(p_wren[lane][0]->read());
    p_wren[lane][2]->write(p_wren[lane][1]->read());
    p_wren[lane][3]->write(p_wren[lane][2]->read());

    p_value[lane][0]->write(extract_predicate_value(inst));
    p_value[lane][1]->write(p_value[lane][0]->read());
    p_value[lane][2]->write(p_value[lane][1]->read());
}

void Hivek::generate_mem_controls(int lane, ControlTable* ct) {
    m_wren[lane][0]->write(ct->m_wren);
    m_wren[lane][1]->write(m_wren[lane][0]->read());

    m_size[lane][0]->write(ct->m_size);
    m_size[lane][1]->write(m_size[lane][0]->read());
}

u32 Hivek::control_address(u32 instruction) {
    int tmp;

    if (is_type_iii(instruction)) {
        if (tmp = is_shadd(instruction)) {
            switch (tmp) {
                case OP_SLL:
                    return 50;
                case OP_SRL:
                    return 51;
                case OP_SRA:
                    return 52;
            }
        } else {
            return extract_op_from_type_iii(instruction) + 18;
        }
    } else if (is_type_i(instruction)) {
        return extract_op_from_type_i(instruction) + 1;
    } else if (is_type_iv(instruction)) {
        return 53;
    } else if (is_type_ii(instruction)) {
        return 17;
    }

    return 0;
}

void Hivek::generate_alu_res_for_lane(int lane) {
    u32 aluOp;
    u32 op_a;
    u32 op_b;

    aluOp = alu_op[lane][1]->read();

    if (alu_pc_vra_sel[lane][1]->read()) {
        op_a = pcs[lane][5]->read();
    } else {
        op_b = vra[lane][1]->read();
    }

    if (alu_vrb_immediate_sel[lane][1]->read()) {
        op_b = immediate[lane][2]->read();
    } else {
        op_b = vrb[lane][1]->read();
    }

    alu_res[lane]->write(alu(aluOp, op_a, op_b));
}

void Hivek::generate_sh_res_for_lane(int lane) {
    u32 sh_t;
    u32 sh_i;
    u32 va;
    u32 vb;
    u32 res;

    sh_t = sh_type[lane][1]->read();
    va   = vra[lane][1]->read();
    vb   = vrb[lane][1]->read();

    if (sh_amount_sel[lane][1]->read()) {
        sh_i = vrb[lane][1]->read() & 0x01F;
    } else {
        sh_i = sh_immediate[lane][2]->read();
    }

    res  = barrel_shifter(sh_t, va, sh_i);

    if (sh_add[lane][1]->read()) {
        res += vb;
    }

    sh_res[lane]->write(res);
}

void Hivek::generate_alu_sh_res_for_lane(int lane) {
    if (alu_sh_sel[lane][2]->read()) {
        alu_sh_res[lane]->write(sh_res[lane]->read());
    } else {
        alu_sh_res[lane]->write(alu_res[lane]->read());
    }
}

void Hivek::writeback_lane(int lane) {
    u32 thread = this->threads[lane][5]->read();
    u32 wren   = this->r_wren[lane][3]->read();
    u32 rc     = this->rc[lane][4]->read();
    u32 vrc;

    if (alu_sh_mem_sel[lane][3]->read()) {
        vrc = mem_res[lane]->read();
    } else {
        vrc = alu_sh_res[lane]->read();
    }
   
    regfile.write(lane, thread, wren, rc, vrc);
}


u32 Hivek::alu(u32 op, u32 a, u32 b) {
    switch (op) {
        case ALU_ADD:
            return a + b;

        case ALU_SUB:
            return a - b;

        case ALU_AND:
            return a & b;

        case ALU_OR:
            return a | b;

        case ALU_NOR:
            return ~(a | b);

        case ALU_XOR:
            return a ^ b;
    }

    return 0;
}

u32 Hivek::barrel_shifter(u32 shift_type, u32 a, u32 shift_ammount) {
    switch (shift_type) {
        case BARREL_SLL:
            return a << shift_ammount;

        case BARREL_SRL:
            return a >> shift_ammount;

        case BARREL_SRA:
            return ((i32) a) >> shift_ammount;
    }
}

u32 Hivek::extract_predicate_value(u32 instruction) {
    return (instruction & 0x04) >> 2;
}

u32 Hivek::extract_predicate_register(u32 instruction) {
    return instruction & 0x03;
}

u32 Hivek::extract_ra(u32 instruction) {
    return (instruction >> 3) & 0x01F;
}

u32 Hivek::extract_rb(u32 instruction) {
    return (instruction >> 8) & 0x01F;
}

u32 Hivek::extract_rc(u32 instruction) {
    return (instruction >> 13) & 0x01F;
}

u32 Hivek::extract_sh_immediate(u32 instruction) {
    return (instruction >> 23) & 0x01F;
}

u32 Hivek::extract_op_from_type_i(u32 instruction) {
    return (instruction >> 23) & 0x0F;
}

u32 Hivek::extract_op_from_type_iii(u32 instruction) {
    return (instruction >> 18) & 0x01F;
}

u32 Hivek::extract_immd_from_type_i(u32 instruction) {
    return (instruction >> 13) & 0x3FF;
}

u32 Hivek::extract_immd_from_type_ii(u32 instruction) {
    return instruction & 0x01FFFFFF;
}

u32 Hivek::extract_immd_from_type_iv(u32 instruction) {
    return (instruction >> 3) & 0x0FFFF;
}

bool Hivek::is_type_i(u32 instruction) {
    return (instruction & TYPE_I_MASK) == TYPE_I_MASK;
}

bool Hivek::is_type_ii(u32 instruction) {
    return (instruction & TYPE_II_MASK) == TYPE_II_MASK;
}

bool Hivek::is_type_iii(u32 instruction) {
    return (instruction & TYPE_III_MASK) == TYPE_III_MASK;
}

bool Hivek::is_type_iv(u32 instruction) {
    return (instruction & TYPE_IV_MASK) == TYPE_IV_MASK;
}

bool Hivek::is_type_v(u32 instruction) {
    return (instruction & TYPE_V_MASK) == TYPE_V_MASK;
}

int Hivek::is_shadd(u32 instruction) {
    return (instruction >> 23) & 0x03;
}

void Hivek::add_waves_to_vcd(VCDMonitor* ptr) {
    ptr->add_register(ctrl_addr[0]);
    ptr->add_register(alu_op[0][0]);
}

Hivek::Hivek() {
#define BUILDM(S,R,C,Z) do { \
for (int i=0; i<R; ++i) { for (int j=0; j<C; ++j) { \
std::stringstream t; t << #S << "{" << i << "}{" << j << "}"; \
S[i][j] = rpool.create_register(t.str(), Z); \
}}} while (0)

#define BUILD(S,R,Z) do { \
for (int i=0; i<R; ++i) { \
std::stringstream t; t << #S << "{" << i << "}"; \
S[i] = rpool.create_register(t.str(), Z); \
}} while (0)

    // TODO
    regfile.set_rpool(&rpool);
    regfile.init();

    BUILD(ctrl_addr, 2, 8);
    BUILD(primary_thread, 2, 3);
    BUILD(pc, 16, 32);
    BUILDM(pcs, 2, 7, 32);
    rt_instructions = rpool.create_register64("rt_instructions", 64);
    nrt_instructions = rpool.create_register64("nrt_instructions", 64);
    BUILDM(threads, 2, 6, 4);

    BUILD(instructions, 2, 32);
    BUILDM(instruction_size, 2, 5, 2);
    BUILDM(instruction_kind, 2, 3, 2);
    BUILD(instruction_rtk, 5, 3);

    BUILDM(alu_op, 2, 2, 4);
    BUILDM(alu_pc_vra_sel, 2, 2, 1);
    BUILDM(alu_vrb_immediate_sel, 2, 2, 1);
    BUILDM(alu_sh_sel, 2, 3, 1);
    BUILDM(alu_sh_mem_sel, 2, 4, 1);

    BUILDM(sh_type, 2, 2, 2);
    BUILDM(sh_amount_sel, 2, 2, 1);
    BUILDM(sh_add, 2, 2, 1);
    BUILDM(sh_immediate, 2, 3, 5);

    ra = rpool.create_register("ra", 5);
    BUILDM(rb, 2, 2, 5);
    BUILDM(rc, 2, 5, 5);
    BUILDM(r_wren, 2, 4, 1);
    BUILD(r_dst_sel, 2, 1);
    BUILD(rb_31_sel, 2, 1);

    BUILDM(p_wren, 2, 4, 1);
    BUILDM(p_value, 2, 3, 1);
    BUILDM(p_rvalue, 2, 2, 1);
    BUILDM(p_register, 2, 2, 2);

    BUILDM(m_wren, 2, 2, 1);
    BUILDM(m_size, 2, 2, 2);

    BUILDM(vra, 2, 2, 32);
    BUILDM(vrb, 2, 2, 32);
    BUILDM(immediate, 2, 3, 32);

    BUILD(alu_res, 2, 32);
    BUILD(sh_res, 2, 32);
    BUILD(alu_sh_res, 2, 32);
    BUILD(mem_res, 2, 32);
}
