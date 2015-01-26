#ifndef HIVEK_MULTITHREAD_EMULATOR_FRAMEBUFFER_H
#define HIVEK_MULTITHREAD_EMULATOR_FRAMEBUFFER_H

#include <SDL2/SDL.h>
#include "Defines.h"

namespace HivekMultithreadEmulator {
    class FrameBuffer {
        private:
            SDL_Window* window;
            SDL_Renderer* renderer;
            SDL_Texture* texture;
            uint16_t* pixels;

        public:
            FrameBuffer(); 
            ~FrameBuffer();
            void write_pixel(u32 addr, u16 pixel);
            void draw();
    };
}

#endif
