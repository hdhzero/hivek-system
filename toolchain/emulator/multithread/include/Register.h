#ifndef REGISTER_H
#define REGISTER_H

#include <vector>
#include <string>
#include "Defines.h"

namespace HivekMultithreadEmulator {
    template<class T>
    class Register {
        private:
            T current;
            T next;

        public:
            int width;
            std::string name;

        public:
            void write(T v) {
                next = v;
            }

            T read() {
                return current;
            }

            void update() {
                current = next;
            }

            void reset() {
                current = 0;
            }

            std::string binvalue() {
                std::string tmp;
                T v = 1;

                for (int i = width - 1; i >= 0; --i) {
                    if (current & (v << i)) {
                        tmp += '1';
                    } else {
                        tmp += '0';
                    }        
                }

                return tmp;
            }
    };
}

#endif
