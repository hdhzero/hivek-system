#include "../include/ControlSignals.h"
using namespace HivekMultithreadEmulator;

void ControlSignals::generate_controls_for_lane(int lane) {
    ControlTable* ct;
    u32 inst;

    generate_alu_controls(lane, ct);
    generate_sh_controls(lane, ct);
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

void ControlSignals::generate_sh_controls(int lane, ControlTable* ct) {
    u32 inst = dpath->get_instruction_in_lane(lane);

    sh_type[lane][0]->write(ct->sh_type);
    sh_type[lane][1]->write(sh_type[lane][0]->read());

    sh_amount_sel[lane][0]->write(ct->sh_amount_sel);
    sh_amount_sel[lane][1]->write(sh_amount_sel[lane][0]->read());

    sh_add[lane][0]->write(ct->sh_add);
    sh_add[lane][1]->write(sh_add[lane][0]->read());

    sh_immd[lane][0]->write(extract_sh_immediate(inst));
    sh_immd[lane][1]->write(sh_immd[lane][0]->read());
    sh_immd[lane][2]->write(sh_immd[lane][1]->read());
}

u32 ControlSignals::extract_sh_immediate(u32 instruction) {
    return (instruction >> 23) & 0x01F;
}
