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
    u32 tmp;
    u32 rt_addr;
    u32 nrt_addr;
    u64 inst;
    bool rt_hits[8];
    bool nrt_hits[8];

    tmp = primary_thread[0]->read();
    primary_thread[1]->write(tmp);
    primary_thread[2]->write(primary_thread[1]->read());

    if (tmp == 7) {
        primary_thread[0]->write(0);
    } else {
        primary_thread[0]->write(tmp + 1);
    }

    rt_addr  = pc[tmp]->read();
    nrt_addr = pc[tmp + 8]->read();

    pcs[0][0]->write(rt_addr);
    pcs[1][0]->write(nrt_addr);

    for (int i = 0; i < 6; ++i) {
        pcs[0][i + 1]->write(pcs[0][i]->read());
        pcs[1][i + 1]->write(pcs[1][i]->read());
    }

    inst = mem->iread64(0, rt_addr, rt_hits);
    rt_instructions->write(inst);

    inst = mem->iread64(1, nrt_addr, nrt_hits);
    nrt_instructions->write(inst);
}

void Hivek::expand() {
    /*if (din.rt_locked.read() && din.nrt_locked.read()) {
        instruction[0]->write(0);
        instruction[1]->write(0);
        rt_nrt[0]->write(0);
        rt_nrt[1]->write(0);
    } else if (din.rt_locked.read()) {
        get_instructions_from_nrt();
    } else {
        get_instructions_from_rt();
    }*/

    get_instructions_from_rt();
    generate_threads();
    generate_instruction_sizes();
    generate_rtks();
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
    generate_jump_res_for_lane(0);
    calculate_next_rt_pc();

    generate_alu_res_for_lane(1);
    generate_sh_res_for_lane(1);
    generate_alu_sh_res_for_lane(1);
    generate_jump_res_for_lane(1);
    calculate_next_nrt_pc();
}

void Hivek::writeback() {
    // TODO
    writeback_lane(0);
    writeback_lane(1);
}

void Hivek::calculate_next_rt_pc() {
    RTNextPCSels sel;
    u32 alu_res;
    u32 pc_res;
    u32 sz_res;
    u32 sz1;
    u32 sz2;
    u32 szs;
    u32 next_pc;
    u32 th;

    u32 rtk;
    u32 k0;
    u32 k1;
    u32 p0;
    u32 p1;

    rtk = this->instruction_rtk[4]->read();
    k0  = this->instruction_kind[0][2]->read();
    k1  = this->instruction_kind[1][2]->read();
    p0  = this->pres[0]->read();
    p1  = this->pres[1]->read();

    sel = rt_next_pc(rtk, k0, k1, p0, p1);

    sz1 = decode_instruction_size(instruction_size[0][4]->read());
    sz2 = decode_instruction_size(instruction_size[1][4]->read());

    if (sel.alu_sel) {
        alu_res = this->alu_res[1]->read();
    } else {
        alu_res = this->alu_res[0]->read();
    }

    if (sel.zero_sz_sel) {
        sz_res = 0;
    } else {
        sz_res = sel.sz_sel ? sz1 + sz2 : sz1;
    }

    pc_res = sel.pc_alu_sel ? pcs[0][6]->read() : alu_res;
    next_pc = pc_res + sz_res;

    th = threads[0][4]->read();

    if (th > 7) {
        th = th - 8;
    }

    pc[threads[0][4]->read()]->write(next_pc);
}

void Hivek::calculate_next_nrt_pc() {
    // TODO
}

RTNextPCSels Hivek::rtnp_make(u32 pc_alu_sel, u32 alu_sel, u32 zero_sz_sel, u32 sz_sel) {
    RTNextPCSels tmp;

    tmp.pc_alu_sel = pc_alu_sel;
    tmp.alu_sel = alu_sel;
    tmp.zero_sz_sel = zero_sz_sel;
    tmp.sz_sel = sz_sel;

    return tmp;
}

RTNextPCSels Hivek::next_rt_pc_rt_rt(u32 k0, u32 k1, u32 p0, u32 p1) {
    u32 kind = (k0 << 2) | k1;
    
    switch (kind) {
    case IK_LAM_LAM:
        return rtnp_make(1, 0, 0, 1);

    case IK_LAM_J:
        return rtnp_make(0, 1, 0, 0);

    case IK_LAM_JR:
        if (p1) {
            return rtnp_make(0, 1, 1, 0);
        } else {
            return rtnp_make(1, 0, 0, 1);
        }

    case IK_LAM_B:
        if (p1) {
            return rtnp_make(0, 1, 0, 0);
        } else {
            return rtnp_make(1, 0, 0, 1);
        }

    case IK_JR_J:
        if (p0) {
            return rtnp_make(0, 0, 1, 0);
        } else {
            return rtnp_make(0, 1, 0, 0);
        }

    case IK_JR_JR:
        if (p0) {
            return rtnp_make(0, 0, 1, 0);
        } else if (p1) {
            return rtnp_make(0, 1, 1, 0);
        } else {
            return rtnp_make(1, 0, 0, 1);
        }

    case IK_JR_B:
        if (p0) {
            return rtnp_make(0, 0, 1, 0);
        } else if (p1) {
            return rtnp_make(0, 1, 0, 0);
        } else {
            return rtnp_make(1, 0, 0, 1);
        } 

    case IK_B_J:
        if (p0) {
            return rtnp_make(0, 0, 1, 0);
        } else {
            return rtnp_make(0, 1, 0, 0);
        }

    case IK_B_JR:
        if (p0) {
            return rtnp_make(0, 0, 1, 0);
        } else if (p1) {
            return rtnp_make(0, 1, 1, 0);
        } else {
            return rtnp_make(1, 0, 0, 1);
        }

    case IK_B_B:
        if (p0) {
            return rtnp_make(0, 0, 1, 0);
        } else if (p1) {
            return rtnp_make(0, 1, 0, 0);
        } else {
            return rtnp_make(1, 0, 0, 1);
        }
    }
}

RTNextPCSels Hivek::next_rt_pc_rt_nrt(u32 k0, u32 p0) {
    switch (k0) {
    case IK_LAM:
        return rtnp_make(1, 0, 0, 0);

    case IK_J:
        return rtnp_make(0, 0, 1, 0);

    case IK_JR:
        if (p0) {
            return rtnp_make(0, 0, 1, 0);
        } else {
            return rtnp_make(1, 0, 0, 0);
        }

    case IK_B:
        if (p0) {
            return rtnp_make(0, 0, 1, 0);
        } else {
            return rtnp_make(1, 0, 0, 0);
        }
    }
}

RTNextPCSels Hivek::next_rt_pc_rt_nop(u32 k0, u32 p0) {
    return next_rt_pc_rt_nrt(k0, p0);
}

RTNextPCSels Hivek::rt_next_pc(u32 rtk, u32 k0, u32 k1, u32 p0, u32 p1) {
    switch (rtk) {
    case RTK_RT_RT:
        return next_rt_pc_rt_rt(k0, k1, p0, p1);

    case RTK_RT_NRT:
        return next_rt_pc_rt_nrt(k0, p0);

    case RTK_RT_NOP:
        return next_rt_pc_rt_nop(k0, p0);
    }
}

void Hivek::get_instructions_from_rt() {
    int size;
    u32 tmp;
    u32 pthread = primary_thread[2]->read();

    tmp = get_first_instruction(rt_instructions->read(), size);
    threads[0][0]->write(pthread);

    instructions[0]->write(tmp);
    instruction_size[0][0]->write(encode_instruction_size(size));

    if (parallel(rt_instructions->read())) {
        tmp = get_second_instruction(rt_instructions->read(), size);
        threads[1][0]->write(pthread);
        instructions[1]->write(tmp);
        instruction_size[1][0]->write(encode_instruction_size(size));
        instruction_rtk[0]->write(RTK_RT_RT);
    } else {
        tmp = get_first_instruction(nrt_instructions->read(), size);
        threads[1][0]->write(pthread + 8);
        instructions[1]->write(tmp);
        instruction_size[1][0]->write(encode_instruction_size(size));
        instruction_rtk[0]->write(RTK_RT_NRT);
    }
}

void Hivek::get_instructions_from_nrt() {
    int size;
    u32 tmp;
    u32 pthread = primary_thread[1]->read() + 8;

    threads[0][0]->write(pthread);
    threads[1][0]->write(pthread);

    tmp = get_first_instruction(nrt_instructions->read(), size);
    instructions[0]->write(tmp);

    if (parallel(nrt_instructions->read())) {
        tmp = get_second_instruction(nrt_instructions->read(), size);
        instructions[1]->write(tmp);
    } else {
        instructions[1]->write(0);
    }
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

        pr = p_register->read();
        p_rvalue[lane][1]->write(regfile.read_pr(lane, pr));
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
    generate_mem_controls(lane, ct);
    generate_immediates(lane, inst);

    instruction_kind[lane][0]->write(ct->instruction_kind);
    instruction_kind[lane][1]->write(instruction_kind[lane][0]->read());
    instruction_kind[lane][2]->write(instruction_kind[lane][1]->read());
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

    if (is_type_iv(inst)) {
        tmp = extract_immd_from_type_iv(inst);

        if (tmp & 0x08000) {
            tmp = 0x0FFFF0000 | tmp;
        }
    } else if (is_type_ii(inst)) {
        tmp = extract_immd_from_type_ii(inst);

        if (tmp & 0x01000000) {
            tmp = 0x0FE000000 | tmp;
        }
    } else if (is_type_i(inst)) {
        tmp = extract_immd_from_type_i(inst);

        if (tmp & 0x200) {
            tmp = 0x0FFFFFC00 | tmp;
        }
    }

    immediate[lane][0]->write(tmp);
    immediate[lane][1]->write(immediate[lane][0]->read());
    immediate[lane][2]->write(immediate[lane][1]->read());
}

void Hivek::generate_jump_res_for_lane(int lane) {
    u32 szs;
    u32 pc;
    u32 rtk = instruction_rtk[4]->read();
    u32 sz1 = decode_instruction_size(instruction_size[0][4]->read());
    u32 sz2 = decode_instruction_size(instruction_size[1][4]->read());

    if (lane == 0) {
        switch (rtk) {
        case RTK_RT_RT:
            szs = sz1 + sz2;
            pc  = pcs[0][6]->read();
            break;

        case RTK_RT_NRT:
        case RTK_RT_NOP:
            szs = sz1;
            pc  = pcs[0][6]->read();
            break;

        case RTK_NRT_NRT:
            szs = sz1 + sz2;
            pc  = pcs[1][6]->read();
            break;

        case RTK_NRT_NOP:
        case RTK_NOP_NOP:
            szs = sz1;
            pc  = pcs[1][6]->read();
        }
    } else {
        switch (rtk) {
        case RTK_RT_RT:
            szs = sz1 + sz2;
            pc  = pcs[0][6]->read();
            break;

        case RTK_RT_NRT:
        case RTK_RT_NOP:
            szs = sz2;
            pc  = pcs[1][6]->read();
            break;

        case RTK_NRT_NRT:
            szs = sz1 + sz2;
            pc  = pcs[1][6]->read();
            break;

        case RTK_NRT_NOP:
        case RTK_NOP_NOP:
            szs = sz1;
            pc  = pcs[1][6]->read();
            break;
        }
    }

    jump_res[lane]->write(pc + szs);
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

    alu_sh_mem_jump_sel[lane][0]->write(ct->alu_sh_mem_jump_sel);
    alu_sh_mem_jump_sel[lane][1]->write(alu_sh_mem_jump_sel[lane][0]->read());
    alu_sh_mem_jump_sel[lane][2]->write(alu_sh_mem_jump_sel[lane][1]->read());
    alu_sh_mem_jump_sel[lane][3]->write(alu_sh_mem_jump_sel[lane][2]->read());
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

    if (p_value[lane][2]->read() != p_rvalue[lane][1]->read()) {
        r_wren[lane][2]->write(0);
    }

    rb[lane][0]->write(extract_rb(inst));
    rb[lane][1]->write(rb[lane][0]->read());

    if (lane == 1) {
        ra->write(extract_ra(inst));
        p_register->write(extract_predicate_register(inst));
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
    u32 flag;

    p_wren[lane][0]->write(ct->p_wren);
    p_wren[lane][1]->write(p_wren[lane][0]->read());
    p_wren[lane][2]->write(p_wren[lane][1]->read());
    p_wren[lane][3]->write(p_wren[lane][2]->read());

    p_value[lane][0]->write(extract_predicate_value(inst));
    p_value[lane][1]->write(p_value[lane][0]->read());
    p_value[lane][2]->write(p_value[lane][1]->read());

    flag = p_value[lane][2]->read() == p_rvalue[lane][1]->read();

    if (!flag) {
        p_wren[lane][2]->write(0);
    }

    pres[lane]->write(flag);
}

void Hivek::generate_mem_controls(int lane, ControlTable* ct) {
    m_wren[lane][0]->write(ct->m_wren);
    m_wren[lane][1]->write(m_wren[lane][0]->read());

    m_size[lane][0]->write(ct->m_size);
    m_size[lane][1]->write(m_size[lane][0]->read());
}

u32 Hivek::control_address(u32 instruction) {
    int tmp;

    if (is_type_iv(instruction)) {
        return 54;
    } else if (is_type_iii(instruction)) {
        if (tmp = is_shadd(instruction)) {
            switch (tmp) {
                case OP_SLL:
                    return 51;
                case OP_SRL:
                    return 52;
                case OP_SRA:
                    return 53;
            }
        } else {
            return extract_op_from_type_iii(instruction) + 19;
        }
    } else if (is_type_ii(instruction)) {
        if (instruction & (1 << 25)) {
            return 18;
        } else {
            return 17;
        }
    } else if (is_type_i(instruction)) {
        return extract_op_from_type_i(instruction) + 1;
    }

    return 0;
}

void Hivek::generate_alu_res_for_lane(int lane) {
    u32 aluOp;
    u32 op_a;
    u32 op_b;

    aluOp = alu_op[lane][1]->read();

    if (alu_pc_vra_sel[lane][1]->read()) {
        u32 rtk = instruction_rtk[3]->read();

        switch (rtk) {
        case RTK_RT_RT:
            op_a = pcs[0][5]->read(); break;

        case RTK_NRT_NRT:
            op_a = pcs[1][5]->read(); break;

        case RTK_RT_NRT:
        case RTK_RT_NOP:
        case RTK_NRT_NOP:
        case RTK_NOP_NOP:
            op_a = lane == 0 ? pcs[0][5]->read() : pcs[1][5]->read(); break;
        }
//        op_a = pcs[lane][5]->read();
    } else {
        op_a = vra[lane][1]->read();
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
    u32 p_wren = this->p_wren[lane][3]->read();
    u32 rc     = this->rc[lane][4]->read();
    u32 vrc;

    if (alu_sh_mem_sel[lane][3]->read()) {
        vrc = mem_res[lane]->read();
    } else {
        vrc = alu_sh_res[lane]->read();
    }

    if (alu_sh_mem_jump_sel[lane][3]->read()) {
        vrc = jump_res[lane]->read();
    }

    regfile.write(lane, thread, wren, rc, vrc);
    regfile.write_pr(lane, p_wren, rc & 0x03, vrc & 0x01);
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

        case ALU_LT:
            return (((i32) a) < ((i32) b));

        case ALU_LTU:
            return a < b;

        case ALU_EQ:
            return a == b;
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
    if (is_type_ii(instruction)) {
        return 1;
    }

    return (instruction & 0x04) >> 2;
}

u32 Hivek::extract_predicate_register(u32 instruction) {
    if (is_type_ii(instruction)) {
        return 0;
    }

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
    return (instruction & 0x078000000) == TYPE_I_MASK;
}

bool Hivek::is_type_ii(u32 instruction) {
    return (instruction & 0x07C000000) == TYPE_II_MASK;
}

bool Hivek::is_type_iii(u32 instruction) {
    return (instruction & 0x07E000000) == TYPE_III_MASK;
}

bool Hivek::is_type_iv(u32 instruction) {
    return (instruction & 0x07F000000) == TYPE_IV_MASK;
}

bool Hivek::is_type_v(u32 instruction) {
    return (instruction & TYPE_V_MASK) == TYPE_V_MASK;
}

int Hivek::is_shadd(u32 instruction) {
    return (instruction >> 23) & 0x03;
}

bool Hivek::parallel(u64 instructions) {
    return (instructions >> 63) == 1;
}

u32 Hivek::expand24(u32 instruction) {
    return 0;
}

u32 Hivek::expand16(u32 instruction) {
    return 0;
}

u32 Hivek::encode_instruction_size(u32 size) {
    switch (size) {
    case 16:
        return SZ_16;

    case 24:
        return SZ_24;

    case 32:
        return SZ_32;

    case 64:
        return SZ_64;
    }
}

u32 Hivek::decode_instruction_size(u32 size) {
    switch (size) {
    case SZ_16:
        return 2;

    case SZ_24:
        return 3;

    case SZ_32:
        return 4;

    case SZ_64:
        return 8;
    }
}

u32 Hivek::get_first_instruction(u64 instructions, int& size) {
    u32 tmp;
    bool flag; 

    tmp = instructions >> 32;
    flag = is_type_i(tmp) || is_type_ii(tmp) || is_type_iv(tmp) || is_type_iii(tmp);

    if (flag) {
        size = 32;
        return tmp;
    } else {
        tmp = instructions >> 40;
        flag = is_type_v(tmp); 

        if (flag) {
            size = 24;
            return expand24(tmp);
        } else {
            tmp = instructions >> 48;
            size = 16;
            return expand16(tmp);
        }
    }
}

void Hivek::dump_registers() {
    regfile.dump_registers();
}

u32 Hivek::get_second_instruction(u64 instructions, int& size) {
    return get_first_instruction(instructions << size, size);
}

void Hivek::add_waves_to_vcd(VCDMonitor* ptr) {
    regfile.add_waves_to_vcd(ptr);
    ptr->add_register(vrb[0][1]);
    ptr->add_register(vra[0][1]);
    ptr->add_register(vrb[0][0]);
    ptr->add_register(vra[0][0]);
    ptr->add_register(alu_res[0]);
    ptr->add_register(alu_res[1]);
    ptr->add_register(r_wren[0][3]);
    ptr->add_register(threads[0][5]);
    ptr->add_register(threads[1][5]);
    ptr->add_register(p_wren[0][3]);
    ptr->add_register(alu_op[0][1]);
    ptr->add_register(pc[0]);
    ptr->add_register(instructions[0]);
    ptr->add_register(instructions[1]);
    ptr->add_register(pcs[0][6]);
    ptr->add_register(pcs[1][6]);
    ptr->add_register(immediate[0][2]);
    ptr->add_register(pres[0]);
    ptr->add_register(pres[1]);
    ptr->add_register(instruction_rtk[0]);
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
    BUILD(primary_thread, 3, 3);
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
    BUILDM(alu_sh_mem_jump_sel, 2, 4, 1);

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
    p_register = rpool.create_register("p_register", 2);
    BUILD(pres, 2, 1);

    BUILDM(m_wren, 2, 2, 1);
    BUILDM(m_size, 2, 2, 2);

    BUILDM(vra, 2, 2, 32);
    BUILDM(vrb, 2, 2, 32);
    BUILDM(immediate, 2, 3, 32);

    BUILD(alu_res, 2, 32);
    BUILD(sh_res, 2, 32);
    BUILD(alu_sh_res, 2, 32);
    BUILD(mem_res, 2, 32);
    BUILD(jump_res, 2, 32);
}
