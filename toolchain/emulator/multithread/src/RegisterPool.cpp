#include "../include/RegisterPool.h"
using namespace HivekMultithreadEmulator;

Register<u32>* RegisterPool::create_register(std::string name, int width) {
    Register<u32>* tmp = new Register<u32>();

    tmp->name = name;
    tmp->width = width;

    register_pool.push_back(tmp);
    return tmp;
}

Register<u64>* RegisterPool::create_register64(std::string name, int width) {
    Register<u64>* tmp = new Register<u64>();

    tmp->name = name;
    tmp->width = width;

    register_pool_64.push_back(tmp);
    return tmp;
}

void RegisterPool::reset() {
    for (int i = 0; i < register_pool.size(); ++i) {
        register_pool[i]->write(0);
        register_pool[i]->update();
    }

    for (int i = 0; i < register_pool_64.size(); ++i) {
        register_pool_64[i]->write(0);
        register_pool_64[i]->update();
    }
}

void RegisterPool::update() {
    for (int i = 0; i < register_pool.size(); ++i) {
        register_pool[i]->update();
    }

    for (int i = 0; i < register_pool_64.size(); ++i) {
        register_pool_64[i]->update();
    }
}

RegisterPool::~RegisterPool() {
    for (int i = 0; i < register_pool.size(); ++i) {
        delete register_pool[i];
    }

    for (int i = 0; i < register_pool_64.size(); ++i) {
        delete register_pool_64[i];
    }
}
