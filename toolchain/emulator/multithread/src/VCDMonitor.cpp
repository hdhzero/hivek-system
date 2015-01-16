#include "../include/VCDMonitor.h"
using namespace HivekMultithreadEmulator;

void VCDMonitor::add_register(Register<u32>* ptr) {
    regs.push_back(ptr);
}

void VCDMonitor::add_register64(Register<u64>* ptr) {
    regs64.push_back(ptr);
}

void VCDMonitor::open(std::string filename) {
    waves.open(filename.c_str());
}

void VCDMonitor::close() {
    waves.close();
}

void VCDMonitor::set_module_name(std::string name) {
    module_name = name;
}

void VCDMonitor::create_vcd_header() {
    unsigned char c;
    unsigned char ct;

    waves << "$date $end\n"
        << "$version $end\n"
        << "$timescale 1ns $end\n"
        << "$scope module " << module_name << " $end\n";

    c = '!';

    for (int i = 0; i < regs.size() && i < ('~' - '!'); ++i) {
        waves << "$var wire " 
            << regs[i]->width << " "
            << c++ << " "
            << regs[i]->name << " $end\n";
    }

    ct = c;

    for (int i = 0; i < regs64.size() && i < ('~' - ct); ++i) {
        waves << "$var wire " 
            << regs64[i]->width << " "
            << c++ << " "
            << regs64[i]->name << " $end\n";
    }

    waves << "$upscope $end\n"
        << "$enddefinitions $end\n"
        << "$dumpvars\n";


    c = '!';

    for (int i = 0; i < regs.size() && i < ('~' - '!'); ++i) {
        if (regs[i]->width == 1) {
            waves << "0" << c << '\n';
        } else {
            waves << "b" << regs[i]->binvalue() << " " << c << '\n';
        }
        ++c;
    }

    ct = c;

    for (int i = 0; i < regs64.size() && i < ('~' - ct); ++i) {
        if (regs64[i]->width == 1) {
            waves << "0" << c << '\n';
        } else {
            waves << "b" << regs64[i]->binvalue() << " " << c << '\n';
        }

        ++c;
    }

    waves << "$end\n";
}

void VCDMonitor::dump_signals(int timestamp) {
    unsigned char c;
    unsigned char ct;
    std::string str;

    waves << "#" << timestamp << '\n';
    c = '!';

    for (int i = 0; i < regs.size() && i < ('~' - '!'); ++i) {
        str = regs[i]->binvalue();

        if (regs[i]->width == 1) {
            waves << str << c << '\n';
        } else {
            waves << "b" << str << " " << c << '\n';
        }

        ++c;
    }

    ct = c;

    for (int i = 0; i < regs64.size() && i < ('~' - ct); ++i) {
        str = regs64[i]->binvalue();

        if (regs64[i]->width == 1) {
            waves << str << c << '\n';
        } else {
            waves << "b" << str << " " << c << '\n';
        }

        ++c;
    }
}
