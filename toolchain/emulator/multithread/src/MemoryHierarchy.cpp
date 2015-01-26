#include "../include/MemoryHierarchy.h"
using namespace HivekMultithreadEmulator;
#include <iostream>
void MemoryHierarchy::reset() {
    rpool.reset();
}

void MemoryHierarchy::cycle() {
    // TODO

    if (render) {
        render = false;
        fb.draw();
    }
}

void MemoryHierarchy::update() {
    rpool.update();
}

//    u16 readv16(u8* ptr) {
//        u16 tmp;
//
//        tmp = ptr[0] << 8;
//        tmp |= ptr[1];
//
//        return tmp;
//    }
//
//    u32 readv32(u8* ptr) {
//        u32 tmp;
//
//        tmp = ptr[0] << 24;
//        tmp |= ptr[1] << 16;
//        tmp |= ptr[2] << 8;
//        tmp |= ptr[3];
//
//        return tmp;
//    }
//
//    u64 readv64(u8* ptr) {
//        u64 tmp = 0;
//
//        tmp |= ptr[0];
//        tmp = tmp << 8;
//
//        tmp |= ptr[1];
//        tmp = tmp << 8;
//
//        tmp |= ptr[2];
//        tmp = tmp << 8;
//
//        tmp |= ptr[3];
//        tmp = tmp << 8;
//
//        tmp |= ptr[4];
//        tmp = tmp << 8;
//
//        tmp |= ptr[5];
//        tmp = tmp << 8;
//
//        tmp |= ptr[6];
//        tmp = tmp << 8;
//
//        tmp |= ptr[7];
//
//        return tmp;
//    }
//
//    void writev16(u8* ptr, u16 v) {
//        ptr[0] = (v >> 8) & 0x0FF;
//        ptr[1] = v & 0x0FF;
//    }
//
//    void writev32(u8* ptr, u32 v) {
//        ptr[0] = (v >> 24) & 0x0FF;
//        ptr[1] = (v >> 16) & 0x0FF;
//        ptr[2] = (v >> 8)  & 0x0FF;
//        ptr[3] = v & 0x0FF;
//    }
//
//    void writev64(u8* ptr, u64 v) {
//        u64 tmp;
//
//        ptr[0] = (v >> 56) & 0x0FF;
//        ptr[1] = (v >> 48) & 0x0FF;
//        ptr[2] = (v >> 40) & 0x0FF;
//        ptr[3] = (v >> 32) & 0x0FF;
//        ptr[4] = (v >> 24) & 0x0FF;
//        ptr[5] = (v >> 16) & 0x0FF;
//        ptr[6] = (v >> 8)  & 0x0FF;
//        ptr[7] = v & 0x0FF;;
//    }


u64 MemoryHierarchy::iread64(int lane, u32 address, bool hits[8]) {
    u64 tmp = 0;

    i_address[lane]->write(address);
    address = i_address[lane]->read();

    for (int i = 0; i < 8; ++i) {
        tmp = tmp << 8;
        tmp = tmp | (ram[address] & 0x0FF);
        ++address;
    }

    return tmp;
}

void MemoryHierarchy::dwrite16(int lane, u32 address, u16 data) {
    static int counter = 0;

    counter = counter + 1;

    if (counter > 640 * 8) {
        render = true;
        counter = 0;
    }

    fb.write_pixel(address, data);
}

void MemoryHierarchy::read_contents_from_file(char* filename) {
    std::ifstream file;
    char c;

    file.open(filename);

    while (file.get(c)) {
        ram.push_back(c);
    }

    ram.resize(4096);
    file.close();
}

MemoryHierarchy::MemoryHierarchy() {
    i_address[0] = rpool.create_register("i_address{0}", 32);
    i_address[1] = rpool.create_register("i_address{1}", 32);
}
