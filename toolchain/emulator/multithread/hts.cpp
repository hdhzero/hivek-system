#include "include/Hivek.h"
#include "include/VCDMonitor.h"
#include "include/MemoryHierarchy.h"

using namespace HivekMultithreadEmulator;

int main(int argc, char** argv) {
    VCDMonitor vcd;
    Hivek hivek;
    MemoryHierarchy mem;

    hivek.set_memory_hierarchy(&mem);
    hivek.add_waves_to_vcd(&vcd);
    mem.read_contents_from_file(argv[1]);
   
    vcd.open("waves.vcd"); 
    vcd.create_vcd_header();

    hivek.reset();
    mem.reset();

//    while (true) {
    for (int i = 0; i < 10000; i += 10) {
//        vcd.dump_signals(i);
        hivek.cycle();
        mem.cycle();
        hivek.update();
        mem.update();
    }

    hivek.dump_registers();
    vcd.close();

    return 0;
}
