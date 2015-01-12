#ifndef HIVEK_MULTITHREAD_EMULATOR_SHIFT_CONTROL_SIGNALS_H
#define HIVEK_MULTITHREAD_EMULATOR_SHIFT_CONTROL_SIGNALS_H

#include "Defines.h"
#include "Register.h"

namespace HivekMultithreadEmulator {
    class ShiftControlSignals {
        public:
            void generate_controls_for_lane(int lane, ControlTable* ct);

        private:
            Register<u32>* sh_type[N_LANES][2];
            Register<u32>* sh_amnt_src[N_LANES][2];
            Register<u32>* sh_add[N_LANES][2];
            Register<u32>* sh_immd[N_LANES][3];
    };
}

#endif
