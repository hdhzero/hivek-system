#include "../include/RegisterFile.h"

using namespace HivekMultithreadEmulator;

void RegisterFile::set_rpool(RegisterPool* rpool) {
    this->rpool = rpool;
}

void RegisterFile::init() {
    ra[0] = rpool->create_register("regfile:ra{0}", 5);
    ra[1] = rpool->create_register("regfile:ra{1}", 5);

    rb[0] = rpool->create_register("regfile:rb{0}", 5);
    rb[1] = rpool->create_register("regfile:rb{1}", 5);

    rc[0] = rpool->create_register("regfile:rc{0}", 5);
    rc[1] = rpool->create_register("regfile:rc{1}", 5);

    vrc[0] = rpool->create_register("regfile:vrc{0}", 32);
    vrc[1] = rpool->create_register("regfile:vrc{1}", 32);

    wren[0] = rpool->create_register("regfile:wren{0}", 1);
    wren[1] = rpool->create_register("regfile:wren{1}", 1);

    thread_r[0] = rpool->create_register("regfile:thread_r{0}", 5);
    thread_r[1] = rpool->create_register("regfile:thread_r{1}", 5);

    thread_w[0] = rpool->create_register("regfile:thread_w{0}", 5);
    thread_w[1] = rpool->create_register("regfile:thread_w{1}", 5);

    pr_r[0] = rpool->create_register("regfile:pr_r{0}", 2);
    pr_r[1] = rpool->create_register("regfile:pr_r{1}", 2);
    
    pr_w[0] = rpool->create_register("regfile:pr_w{0}", 2);
    pr_w[1] = rpool->create_register("regfile:pr_w{1}", 2);

    pr_v[0] = rpool->create_register("regfile:pr_v{0}", 2);
    pr_v[1] = rpool->create_register("regfile:pr_v{1}", 2);

    pr_wren[0] = rpool->create_register("regfile:pr_wren{0}", 2);
    pr_wren[1] = rpool->create_register("regfile:pr_wren{1}", 2);

    for (int i = 0; i < 16; ++i) {
        registers[i][0] = 0;
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

    if (rc != 0 && wren) {
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

