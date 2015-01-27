#include "../include/RegisterFile.h"
#include <iostream>

using namespace HivekMultithreadEmulator;

void RegisterFile::set_rpool(RegisterPool* rpool) {
    this->rpool = rpool;
}

void RegisterFile::add_waves_to_vcd(VCDMonitor* ptr) {
    ptr->add_register(rc[0]);
    ptr->add_register(rc[1]);
    ptr->add_register(wren[0]);
    ptr->add_register(wren[1]);
    ptr->add_register(vrc[0]);
    ptr->add_register(vrc[1]);
    ptr->add_register(thread_w[0]);
    ptr->add_register(thread_w[1]);
    ptr->add_register(pr_wren[0]);
    ptr->add_register(pr_w[0]);
    ptr->add_register(pr_v[0]);
}

void RegisterFile::init() {
    ra[0] = rpool->create_register("regfileRa{0}", 5);
    ra[1] = rpool->create_register("regfileRa{1}", 5);

    rb[0] = rpool->create_register("regfileRb{0}", 5);
    rb[1] = rpool->create_register("regfileRb{1}", 5);

    rc[0] = rpool->create_register("regfileRc{0}", 5);
    rc[1] = rpool->create_register("regfileRc{1}", 5);

    vrc[0] = rpool->create_register("regfileVrc{0}", 32);
    vrc[1] = rpool->create_register("regfileVrc{1}", 32);

    wren[0] = rpool->create_register("regfileWren{0}", 1);
    wren[1] = rpool->create_register("regfileWren{1}", 1);

    thread_r[0] = rpool->create_register("regfileThread_r{0}", 5);
    thread_r[1] = rpool->create_register("regfileThread_r{1}", 5);

    thread_w[0] = rpool->create_register("regfileThread_w{0}", 5);
    thread_w[1] = rpool->create_register("regfileThread_w{1}", 5);

    pr_r[0] = rpool->create_register("regfilePr_r{0}", 2);
    pr_r[1] = rpool->create_register("regfilePr_r{1}", 2);
    
    pr_w[0] = rpool->create_register("regfilePr_w{0}", 2);
    pr_w[1] = rpool->create_register("regfilePr_w{1}", 2);

    pr_v[0] = rpool->create_register("regfilePr_v{0}", 2);
    pr_v[1] = rpool->create_register("regfilePr_v{1}", 2);

    pr_wren[0] = rpool->create_register("regfilePr_wren{0}", 1);
    pr_wren[1] = rpool->create_register("regfilePr_wren{1}", 1);

    for (int i = 0; i < 16; ++i) {
        pr_registers[i][0] = 1;
        pr_registers[i][1] = 0;
        pr_registers[i][2] = 0;
        pr_registers[i][3] = 0;

        registers[i][0] = 0;

        for (int j = 0; j < 32; ++j) {
            registers[i][j] = 0;
            registers[i][10] = 128512071;
        }
    }
}

void RegisterFile::write(int lane, u32 thread, u32 wren, u32 rc, u32 vrc) {
    this->thread_w[lane]->write(thread);
    this->wren[lane]->write(wren);
    this->rc[lane]->write(rc);
    this->vrc[lane]->write(vrc);

    thread = this->thread_w[lane]->read();
    wren   = this->wren[lane]->read();
    vrc    = this->vrc[lane]->read();
    rc     = this->rc[lane]->read();

    if (rc != 0 && wren == 1) {
        registers[thread][rc] = vrc;
    }
}

void RegisterFile::write_pr(int lane, u32 wren, u32 pr, u32 prv) {
    u32 th;

    this->pr_wren[lane]->write(wren);
    this->pr_w[lane]->write(pr);
    this->pr_v[lane]->write(prv);

    th   = this->thread_w[lane]->read();
    wren = this->pr_wren[lane]->read();
    pr   = this->pr_w[lane]->read();
    prv  = this->pr_v[lane]->read();

    if (pr != 0 && wren) {
        pr_registers[th][pr] = prv;
    }
}

u32 RegisterFile::read_ra(int lane, u32 thread, u32 ra) {
    this->ra[lane]->write(ra);
    this->thread_r[lane]->write(thread);

    return get_vra(lane);
}

u32 RegisterFile::read_rb(int lane, u32 thread, u32 rb) {
    this->rb[lane]->write(rb);
    this->thread_r[lane]->write(thread);

    return get_vrb(lane);
}

u32 RegisterFile::read_pr(int lane, u32 pr) {
    u32 pr_r = this->pr_r[lane]->read();
    u32 th   = this->thread_r[lane]->read();

    this->pr_r[lane]->write(pr);

    return pr_registers[th][pr_r];
}

u32 RegisterFile::get_vra(int lane) {
    u32 thread = this->thread_r[lane]->read();
    u32 ra = this->ra[lane]->read();

    return registers[thread][ra];
}

u32 RegisterFile::get_vrb(int lane) {
    u32 thread = this->thread_r[lane]->read();
    u32 rb = this->rb[lane]->read();

    return registers[thread][rb];
}

void RegisterFile::dump_registers() {
    for (int i = 0; i < 16; ++i) {
        std::cout << "thread " << i << ":\n    ";

        for (int j = 0; j < 32; ++j) {
            if (j == 15) std::cout << std::endl << "    ";
            std::cout << j << ":" << (i32) registers[i][j] << "  ";
        }

        std::cout << '\n';
    }
}
