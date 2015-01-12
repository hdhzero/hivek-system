#ifndef HIVEK_MULTITHREAD_EMULATOR_CONTROL_SIGNALS_H
#define HIVEK_MULTITHREAD_EMULATOR_CONTROL_SIGNALS_H

#include "AluControls.h"
#include "ShControls.h"
#include "RegisterControlSignals.h"
#include "PredicateControlSignals.h"

namespace HivekMultithreadEmulator {
    class ControlSignals {
        private:
            AluControls alu_ctrls;
            ShControls sh_ctrls;
            RegisterControlSignals r_ctrls;
            PredicateControlSignals p_ctrls;

        public:
            void generate_controls_for_lane(int lane);
            
    };
}

#endif
