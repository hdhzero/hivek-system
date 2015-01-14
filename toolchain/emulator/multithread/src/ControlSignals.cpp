#include "../include/ControlSignals.h"
using namespace HivekMultithreadEmulator;

void ControlSignals::generate_controls_for_lane(int lane) {
    ControlTable* ct;
    u32 inst;

    inst = dpath->get_instruction_in_lane(lane);
    ctrl_addr[lane]->write(control_address(inst));
    ct = &control_table[ctrl_addr[lane]->read()];

    generate_alu_controls(lane, ct);
    generate_sh_controls(lane, ct, inst);
    generate_reg_controls(lane, ct, inst);
    // TODO
/*

    inst = datapath->get_instruction_in_lane(lane);instruction[lane]->read();
    ctrl_addr[lane]->write(control_address(inst));
    ct = &control_table[ctrl_addr[lane]->read()];

    alu_ctrls.generate_controls_for_lane(lane, ct);
    sh_ctrls.generate_controls_for_lane(lane, ct);
    r_ctrls.generate_controls_for_lane(lane);
    p_ctrls.generate_controls_for_lane(lane);
*/
}

u32 ControlSignals::get_alu_op(int lane) {
    return alu_op[lane][1]->read();
}

u32 ControlSignals::get_alu_pc_vra_sel(int lane) {
    return alu_pc_vra_sel[lane][1]->read();
}

u32 ControlSignals::get_alu_vrb_immediate_sel(int lane) {
    return alu_vrb_immediate_sel[lane][1]->read();
}

u32 ControlSignals::get_alu_sh_sel(int lane) {
    return alu_sh_sel[lane][2]->read();
}

u32 ControlSignals::get_sh_type(int lane) {
    return sh_type[lane][1]->read();;
}

u32 ControlSignals::get_sh_amount_sel(int lane) {
    return sh_amount_sel[lane][1]->read();
}

u32 ControlSignals::get_sh_add(int lane) {
    return sh_add[lane][1]->read();
}

u32 ControlSignals::get_sh_immediate(int lane) {
    return sh_immediate[lane][2]->read() & 0x01F;
}

void ControlSignals::generate_alu_controls(int lane, ControlTable* ct) {
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

void ControlSignals::generate_sh_controls(int lane, ControlTable* ct, u32 inst) {
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

void ControlSignals::generate_reg_controls(int lane, ControlTable* ct, u32 inst) {
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

u32 ControlSignals::extract_sh_immediate(u32 instruction) {
    return (instruction >> 23) & 0x01F;
}

u32 ControlSignals::control_address(u32 instruction) {
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

u32 ControlSignals::extract_ra(u32 instruction) {
    return (instruction >> 3) & 0x01F;
}

u32 ControlSignals::extract_rb(u32 instruction) {
    return (instruction >> 8) & 0x01F;
}

u32 ControlSignals::extract_rc(u32 instruction) {
    return (instruction >> 13) & 0x01F;
}

u32 ControlSignals::extract_op_from_type_i(u32 instruction) {
    return (instruction >> 23) & 0x0F;
}

u32 ControlSignals::extract_op_from_type_iii(u32 instruction) {
    return (instruction >> 18) & 0x01F;
}

bool ControlSignals::is_type_i(u32 instruction) {
    return (instruction & TYPE_I_MASK) == TYPE_I_MASK;
}

bool ControlSignals::is_type_ii(u32 instruction) {
    return (instruction & TYPE_II_MASK) == TYPE_II_MASK;
}

bool ControlSignals::is_type_iii(u32 instruction) {
    return (instruction & TYPE_III_MASK) == TYPE_III_MASK;
}

bool ControlSignals::is_type_iv(u32 instruction) {
    return (instruction & TYPE_IV_MASK) == TYPE_IV_MASK;
}

bool ControlSignals::is_type_v(u32 instruction) {
    return (instruction & TYPE_V_MASK) == TYPE_V_MASK;
}

int ControlSignals::is_shadd(u32 instruction) {
    return (instruction >> 23) & 0x03;
}

