#include <cstdint>

namespace GFX{
    void init();
    void clear(uint32_t color);
    void swapBuffers();
    void drawRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, uint32_t color);
    void drawImage(unsigned int x, unsigned int y, unsigned int w, unsigned int h, uint32_t image[]);
    void drawInt(int number, int x, int y);
}