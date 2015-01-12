#include "ControlSignals.h"
using namespace HivekMultithreadEmulator;

void DecodeControlSignals::generate_controls_for_lane(int lane) {
    ControlTable* ct;
    u32 inst;

    inst = datapath->get_instruction_in_lane(lane);instruction[lane]->read();
    ctrl_addr[lane]->write(control_address(inst));
    ct = &control_table[ctrl_addr[lane]->read()];

    alu_ctrls.generate_controls_for_lane(lane, ct);
    sh_ctrls.generate_controls_for_lane(lane, ct);
    r_ctrls.generate_controls_for_lane(lane);
    p_ctrls.generate_controls_for_lane(lane);
}

